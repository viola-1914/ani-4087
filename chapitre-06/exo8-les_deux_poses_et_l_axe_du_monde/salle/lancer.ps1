# lancer.ps1 — chapitre 06, exercice 8 : les deux poses et l'axe du monde.
#
# Compile la salle avec ..\poses.cpp, calcule les yeux des trois poses
# (sans ouvrir la salle) et ecrit le resultat dans ..\poses.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "poses.cpp") "src\poses.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSallePoses.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

if (Test-Path "poses_sortie.txt") { Remove-Item "poses_sortie.txt" }
$env:MASALLE_CALCUL_SEUL = "1"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_CALCUL_SEUL

if (-not (Test-Path "poses_sortie.txt")) {
    Write-Host "Aucun resultat ecrit." -ForegroundColor Red
    exit 1
}
Get-Content "poses_sortie.txt" | Set-Content (Join-Path ".." "poses.txt") -Encoding ASCII
Write-Host "`nResultat (copie dans ..\poses.txt) :" -ForegroundColor Green
Get-Content "poses_sortie.txt"
