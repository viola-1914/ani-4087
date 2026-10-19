# lancer.ps1 — chapitre 06, exercice 4 : les crochets de test.
#
# Compile la salle avec ..\tete.cpp et l'ouvre normalement (sans test).
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

# tete.cpp vit dans le dossier de l'exercice (..\) : on en copie la
# version a jour dans src\ avant chaque construction.
Copy-Item (Join-Path ".." "tete.cpp") "src\tete.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleTete.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Write-Host "Ouverture de la salle : la souris tourne la tete. Echap ferme." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
