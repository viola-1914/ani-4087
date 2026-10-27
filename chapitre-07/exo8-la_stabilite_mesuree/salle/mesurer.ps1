# mesurer.ps1 - chapitre 07, exercice 8 : la stabilite mesuree.
#
# Compile la salle avec ..\stabilite.cpp, puis la lance deux fois ; chaque
# fois elle ecarte les 30 premieres images, mesure UNE MINUTE et se ferme seule :
#   1. telle quelle : elle attend l'ecran          -> ligne "ecran"
#   2. avec NK_VK_MAILBOX=1 : sans cette attente     -> ligne "sans attente"
# Ecrit ..\stabilite.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1
# Pendant les deux minutes, ne touchez ni la souris ni le clavier, et ne
# lancez rien d'autre : un arret de quelques secondes ruinerait l'ecart type.

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

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "stabilite.cpp") "src\stabilite.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleStabilite.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

function Mesurer([string]$nom, [bool]$mailbox) {
    if (Test-Path "stabilite_sortie.txt") { Remove-Item "stabilite_sortie.txt" }
    if ($mailbox) { $env:NK_VK_MAILBOX = "1" } else { Remove-Item Env:NK_VK_MAILBOX -ErrorAction SilentlyContinue }
    Write-Host "Mesure '$nom' : une minute, ne touchez a rien..." -ForegroundColor Green
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    Remove-Item Env:NK_VK_MAILBOX -ErrorAction SilentlyContinue
    if (-not (Test-Path "stabilite_sortie.txt")) {
        Write-Host "  aucune sortie" -ForegroundColor Red
        exit 1
    }
    $lignes = Get-Content "stabilite_sortie.txt"
    Copy-Item "stabilite_sortie.txt" "stabilite_$($nom -replace ' ', '_').txt" -Force
    return $lignes
}

$ecran = Mesurer "ecran" $false
$sansAttente = Mesurer "sans attente" $true

@($ecran | Where-Object { $_ -match "^ecran : " }) + @($sansAttente | Where-Object { $_ -match "^sans attente : " }) |
    Set-Content (Join-Path ".." "stabilite.txt") -Encoding ASCII

Write-Host "`nstabilite.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "stabilite.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$ecran | Where-Object { $_ -match "^\(info\)" } | ForEach-Object { "ecran        $_" }
$sansAttente | Where-Object { $_ -match "^\(info\)" } | ForEach-Object { "sans attente $_" }
