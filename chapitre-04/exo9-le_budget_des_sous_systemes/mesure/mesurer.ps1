# mesurer.ps1 — chapitre 04, exercice 9.
#
# Relance VRAIMENT MaSalleBudget vingt fois : dix avec NK_SS_ALL, puis dix avec
# NK_SS_RENDER3D | NK_SS_SHADOW. Chaque lancement est un nouveau processus,
# donc un vrai demarrage. Ecrit ensuite ..\mesures.txt au format demande.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

param(
    # Code source de Nkentseu : il contient Resources\NKRenderer (shaders).
    [string]$NkentseuSource = "C:\Users\HP\Documents\GitHub\Nkentseu"
)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

# Le renderer cherche Resources\NKRenderer dans le dossier courant. Le kit
# ne contient que les bibliotheques : on relie ici le dossier Resources du
# code source par une jonction (aucune copie, rien a telecharger).
$sourceResources = Join-Path $NkentseuSource "Resources"
if (-not (Test-Path (Join-Path $sourceResources "NKRenderer"))) {
    Write-Host "Introuvable : $sourceResources\NKRenderer" -ForegroundColor Red
    Write-Host "Relancez avec : .\mesurer.ps1 -NkentseuSource <dossier du code source de Nkentseu>" -ForegroundColor Red
    exit 1
}
if (-not (Test-Path "Resources")) {
    cmd /c mklink /J Resources "$sourceResources" | Out-Null
}
if (-not (Test-Path "Resources\NKRenderer")) {
    Write-Host "Impossible de relier le dossier Resources." -ForegroundColor Red
    exit 1
}

Write-Host "Compilation en Release..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue : corrigez-la avant de mesurer." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleBudget.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1
if ($null -eq $exe) {
    Write-Host "MaSalleBudget.exe introuvable dans Build\ (Release)." -ForegroundColor Red
    exit 1
}
Write-Host "Programme : $($exe.FullName)"

# On repart d'un fichier vide : seules les vingt mesures de cette seance comptent.
foreach ($ancien in @("mesures_brutes.txt", "diagnostic.txt", "api_retenue.txt")) {
    if (Test-Path $ancien) {
        Remove-Item $ancien
    }
}

# Lancement d'essai, NON compte : il cherche l'interface graphique avec
# laquelle le renderer demarre (Vulkan, OpenGL, DX11 puis DX12). Les vingt
# mesures l'imposent ensuite, pour qu'aucune ne paie un essai rate.
Write-Host "Recherche de l'interface graphique qui fonctionne..."
$sonde = Start-Process -FilePath $exe.FullName -ArgumentList "ALL" -WorkingDirectory $PSScriptRoot -Wait -PassThru
if ($sonde.ExitCode -ne 0 -or -not (Test-Path "api_retenue.txt")) {
    Write-Host "Aucune interface graphique ne permet de demarrer le renderer." -ForegroundColor Red
    if (Test-Path "diagnostic.txt") {
        Write-Host "`nContenu de diagnostic.txt :"
        Get-Content "diagnostic.txt"
    }
    exit 1
}
$api = (Get-Content "api_retenue.txt" | Select-Object -First 1).Trim()
Write-Host "Interface retenue : $api" -ForegroundColor Green
Remove-Item "mesures_brutes.txt" -ErrorAction SilentlyContinue

$echec = $false
foreach ($config in @("ALL", "SALLE")) {
    if ($echec) {
        break
    }
    for ($essai = 1; $essai -le 10; $essai++) {
        Write-Host "Demarrage $config, essai $essai..."
        $processus = Start-Process -FilePath $exe.FullName -ArgumentList @($config, "API=$api") -WorkingDirectory $PSScriptRoot -Wait -PassThru
        if ($processus.ExitCode -ne 0) {
            # Inutile de relancer dix-neuf fois un programme qui echoue :
            # on s'arrete au premier echec et on montre sa cause.
            Write-Host "  -> echec, code $($processus.ExitCode) : arret des mesures." -ForegroundColor Yellow
            $echec = $true
            break
        }
        Start-Sleep -Milliseconds 500
    }
}

if ($echec -or -not (Test-Path "mesures_brutes.txt")) {
    Write-Host "Mesures interrompues." -ForegroundColor Red
    if (Test-Path "diagnostic.txt") {
        Write-Host "`nContenu de diagnostic.txt :"
        Get-Content "diagnostic.txt"
    }
    if (Test-Path "logs\app.log") {
        Write-Host "`nFin du journal du moteur (logs\app.log) :"
        Get-Content "logs\app.log" -Tail 40
    }
    exit 1
}

$lignes = @(Get-Content "mesures_brutes.txt" | Where-Object { $_.Trim() -ne "" })
if ($lignes.Count -ne 20) {
    Write-Host "Attendu 20 mesures, obtenu $($lignes.Count). Relancez le script." -ForegroundColor Red
    exit 1
}

$sortie = New-Object System.Collections.Generic.List[string]
$tempsAll = New-Object System.Collections.Generic.List[string]
$tempsSalle = New-Object System.Collections.Generic.List[string]
$numeroAll = 0
$numeroSalle = 0
foreach ($ligne in $lignes) {
    $morceaux = $ligne.Trim().Split(" ")
    if ($morceaux[0] -eq "ALL") {
        $numeroAll++
        $sortie.Add("config : ALL, essai : $numeroAll, ms : $($morceaux[1])")
        $tempsAll.Add($morceaux[1])
    } else {
        $numeroSalle++
        $sortie.Add("config : RENDER3D|SHADOW, essai : $numeroSalle, ms : $($morceaux[1])")
        $tempsSalle.Add($morceaux[1])
    }
}

# UTF-8 sans BOM et fins de ligne Unix : le correcteur lit le fichier tel quel.
$utf8 = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText((Join-Path $PSScriptRoot "..\mesures.txt"), (($sortie -join "`n") + "`n"), $utf8)
Write-Host "mesures.txt ecrit (20 lignes)." -ForegroundColor Green

# Entree prete pour budget.cpp, avec VOS mesures.
$entree = "tout 1 ALL`n" + ($tempsAll -join " ") + "`nsalle 2 RENDER3D SHADOW`n" + ($tempsSalle -join " ") + "`n"
[System.IO.File]::WriteAllText((Join-Path $PSScriptRoot "budget_entree.txt"), $entree, $utf8)

if (Get-Command clang++ -ErrorAction SilentlyContinue) {
    clang++ -std=c++17 -Wall ..\budget.cpp -o budget.exe
    Write-Host "`nResume de VOS mesures :" -ForegroundColor Green
    Get-Content "budget_entree.txt" | .\budget.exe
}

Write-Host "`nContenu de mesures.txt :"
Get-Content "..\mesures.txt"