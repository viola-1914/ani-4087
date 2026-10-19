# lancer.ps1 — chapitre 06, exercice 5 : les deux ordres et la matrice.
#
# Compile la salle avec ..\ordres.cpp , calcule les trois regards et ouvre la salle.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1
#

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

# ordres.cpp vit dans le dossier de l'exercice (..\) : on en copie la
# version a jour dans src\ avant chaque construction.
Copy-Item (Join-Path ".." "ordres.cpp") "src\ordres.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleOrdres.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Write-Host "Ouverture de la salle. Fermez-la avec Echap pour voir les regards." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait

Write-Host "`nRegards (regards_sortie.txt) :" -ForegroundColor Green
if (Test-Path "regards_sortie.txt") {
    Get-Content "regards_sortie.txt"
} else {
    Write-Host "Aucun regard ecrit : le programme s'est arrete avant le calcul." -ForegroundColor Red
}
