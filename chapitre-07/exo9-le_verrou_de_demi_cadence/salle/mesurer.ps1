# mesurer.ps1 - chapitre 07, exercice 9 : le verrou de demi-cadence.
#
# Compile la salle avec ..\verrou.cpp et la lance une fois, avec MASALLE_HZ =
# la frequence de l'ecran (60 Hz dans les parametres d'affichage de Windows).
# La salle ajoute des copies jusqu'a ne plus tenir la periode de l'ecran, mesure
# mille intervalles sans verrou puis mille avec, et se ferme seule (une a deux
# minutes). Ecrit ..\verrou.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1
# Ne touchez a rien et ne lancez rien d'autre pendant la mesure.

param(
    # Frequence de l'ecran, lue dans les parametres d'affichage de Windows.
    [string]$Hz = "60",
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
Copy-Item (Join-Path ".." "verrou.cpp") "src\verrou.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleVerrou.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

if (Test-Path "verrou_sortie.txt") { Remove-Item "verrou_sortie.txt" }
$env:MASALLE_HZ = $Hz
Write-Host "Mesure en cours (une a deux minutes, ne touchez a rien)..." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_HZ -ErrorAction SilentlyContinue

if (-not (Test-Path "verrou_sortie.txt")) {
    Write-Host "Aucune sortie." -ForegroundColor Red
    exit 1
}
$lignes = Get-Content "verrou_sortie.txt"
$lignes | Where-Object { $_ -notmatch "^\(info\)" } | Set-Content (Join-Path ".." "verrou.txt") -Encoding ASCII

Write-Host "`nverrou.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "verrou.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$lignes | Where-Object { $_ -match "^\(info\)" }
