# capturer.ps1 — chapitre 04, exercice 10.
#
# Compile MaSalleLumiere, puis rend la salle quatre fois et enregistre :
#   reference.png  : direction (-0,4 ; -1 ; -0,3), intensite 3, ombres actives
#   direction.png  : seule la direction change (soleil du soir)
#   intensite.png  : seule l'intensite change (1 au lieu de 3)
#   ombre.png      : seul castShadow change (false)
# Les images sont deplacees dans le dossier de l'exercice (..\).
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\capturer.ps1

param(
    # Code source de Nkentseu : il contient Resources\NKRenderer (shaders).
    [string]$NkentseuSource = "C:\Users\HP\Documents\GitHub\Nkentseu"
)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

$sourceResources = Join-Path $NkentseuSource "Resources"
if (-not (Test-Path (Join-Path $sourceResources "NKRenderer"))) {
    Write-Host "Introuvable : $sourceResources\NKRenderer" -ForegroundColor Red
    exit 1
}
if (-not (Test-Path "Resources")) {
    cmd /c mklink /J Resources "$sourceResources" | Out-Null
}

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleLumiere.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

$reglages = @(
    @{ Argument = "REFERENCE"; Fichier = "reference.png" },
    @{ Argument = "DIRECTION"; Fichier = "direction.png" },
    @{ Argument = "INTENSITE"; Fichier = "intensite.png" },
    @{ Argument = "OMBRE"; Fichier = "ombre.png" }
)

$echecs = 0
foreach ($reglage in $reglages) {
    if (Test-Path $reglage.Fichier) {
        Remove-Item $reglage.Fichier
    }
    Write-Host "Capture $($reglage.Fichier)..."
    $processus = Start-Process -FilePath $exe.FullName -ArgumentList $reglage.Argument -WorkingDirectory $PSScriptRoot -Wait -PassThru
    if ($processus.ExitCode -ne 0 -or -not (Test-Path $reglage.Fichier) -or (Get-Item $reglage.Fichier).Length -eq 0) {
        Write-Host "  -> echec (code $($processus.ExitCode))" -ForegroundColor Red
        $echecs++
        continue
    }
    Move-Item $reglage.Fichier (Join-Path ".." $reglage.Fichier) -Force
    Write-Host "  -> ..\$($reglage.Fichier)" -ForegroundColor Green
}

if ($echecs -gt 0) {
    Write-Host "`n$echecs capture(s) en echec. Fin du journal du moteur :" -ForegroundColor Red
    if (Test-Path "logs\app.log") {
        Get-Content "logs\app.log" -Tail 30
    }
    exit 1
}

Write-Host "`nLes quatre captures sont dans le dossier de l'exercice." -ForegroundColor Green
Invoke-Item (Join-Path ".." "reference.png")
