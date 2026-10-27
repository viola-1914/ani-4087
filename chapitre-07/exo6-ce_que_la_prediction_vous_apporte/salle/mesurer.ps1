# mesurer.ps1 - chapitre 07, exercice 6 : ce que la prediction vous apporte.
#
# Compile la salle avec ..\gestes.cpp et lance le calcul des trois gestes
# (sans ouvrir la salle). Ecrit ..\gestes.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "gestes.cpp") "src\gestes.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleGestes.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

if (Test-Path "gestes_sortie.txt") { Remove-Item "gestes_sortie.txt" }
$env:MASALLE_CALCUL_SEUL = "1"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_CALCUL_SEUL -ErrorAction SilentlyContinue

if (-not (Test-Path "gestes_sortie.txt")) {
    Write-Host "Aucune sortie." -ForegroundColor Red
    exit 1
}
$lignes = Get-Content "gestes_sortie.txt"
$lignes | Where-Object { $_ -notmatch "^\(info\)" } | Set-Content (Join-Path ".." "gestes.txt") -Encoding ASCII

Write-Host "`ngestes.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "gestes.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$lignes | Where-Object { $_ -match "^\(info\)" }
