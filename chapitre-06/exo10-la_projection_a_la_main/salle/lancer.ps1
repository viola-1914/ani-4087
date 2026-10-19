# lancer.ps1 — chapitre 06, exercice 10 : la projection a la main.
#
# Compile la salle avec ..\projection.cpp, calcule les deux epreuves (sans
# ouvrir la salle), ecrit ..\ecarts.txt et affiche aussi les matrices.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "projection.cpp") "src\projection.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleProjection.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

foreach ($f in @("ecarts_sortie.txt", "matrices_sortie.txt")) { if (Test-Path $f) { Remove-Item $f } }
$env:MASALLE_CALCUL_SEUL = "1"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_CALCUL_SEUL

if (-not (Test-Path "ecarts_sortie.txt")) {
    Write-Host "Aucun resultat ecrit." -ForegroundColor Red
    exit 1
}
Get-Content "ecarts_sortie.txt" | Set-Content (Join-Path ".." "ecarts.txt") -Encoding ASCII
Write-Host "`nEcarts (copies dans ..\ecarts.txt) :" -ForegroundColor Green
Get-Content "ecarts_sortie.txt"
Write-Host "`nMatrices et verifications :" -ForegroundColor Green
Get-Content "matrices_sortie.txt"
