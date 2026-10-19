# lancer.ps1 — compile puis ouvre ma salle en 3D.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1
#
# Essayez Echap (rendre) avant tout le reste. La croix ferme la fenetre.

param(
    # Code source de Nkentseu : il contient Resources\NKRenderer (shaders).
    [string]$NkentseuSource = "C:\Users\HP\Documents\GitHub\Nkentseu"
)

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

# Le renderer cherche Resources\NKRenderer dans le dossier courant : on relie
# celui du code source par une jonction (aucune copie).
$sourceResources = Join-Path $NkentseuSource "Resources"
if (-not (Test-Path (Join-Path $sourceResources "NKRenderer"))) {
    Write-Host "Introuvable : $sourceResources\NKRenderer" -ForegroundColor Red
    exit 1
}
if (-not (Test-Path "Resources")) {
    cmd /c mklink /J Resources "$sourceResources" | Out-Null
}

# pointeur.cpp et pointeur.h vivent dans le dossier de l'exercice (..\) :
# on en copie la version a jour dans src\ avant chaque construction.
Copy-Item (Join-Path ".." "pointeur.cpp") "src\pointeur.cpp" -Force
Copy-Item (Join-Path ".." "pointeur.h") "src\pointeur.h" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalle3D.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Write-Host "Ouverture de la salle (Echap rend le pointeur, Entree le reprend, la croix ferme)..." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
