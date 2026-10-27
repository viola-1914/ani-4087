# mesurer.ps1 - chapitre 07, exercice 10 : un meilleur modele.
#
# Compile la salle avec ..\modeles.cpp et lance le calcul des quatre enregistrements
# (sans ouvrir la salle). Ecrit ..\modeles.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "modeles.cpp") "src\modeles.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleModeles.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

if (Test-Path "modeles_sortie.txt") { Remove-Item "modeles_sortie.txt" }
$env:MASALLE_CALCUL_SEUL = "1"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_CALCUL_SEUL -ErrorAction SilentlyContinue

if (-not (Test-Path "modeles_sortie.txt")) {
    Write-Host "Aucune sortie." -ForegroundColor Red
    exit 1
}
$lignes = Get-Content "modeles_sortie.txt"
$lignes | Where-Object { $_ -notmatch "^\(info\)" } | Set-Content (Join-Path ".." "modeles.txt") -Encoding ASCII

Write-Host "`nmodeles.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "modeles.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$lignes | Where-Object { $_ -match "^\(info\)" }
