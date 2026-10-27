# mesurer.ps1 - chapitre 07, exercice 7 : la latence injectee et la borne.
#
# Compile la salle avec ..\latence.cpp et lance le calcul des cinq latences et de la borne
# (sans ouvrir la salle). Ecrit ..\latences.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "latence.cpp") "src\latence.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleLatence.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

if (Test-Path "latence_sortie.txt") { Remove-Item "latence_sortie.txt" }
$env:MASALLE_CALCUL_SEUL = "1"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_CALCUL_SEUL -ErrorAction SilentlyContinue

if (-not (Test-Path "latence_sortie.txt")) {
    Write-Host "Aucune sortie." -ForegroundColor Red
    exit 1
}
$lignes = Get-Content "latence_sortie.txt"
$lignes | Where-Object { $_ -notmatch "^\(info\)" } | Set-Content (Join-Path ".." "latences.txt") -Encoding ASCII

Write-Host "`nlatences.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "latences.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$lignes | Where-Object { $_ -match "^\(info\)" }
