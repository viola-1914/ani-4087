# mesurer.ps1 — chapitre 08, exercice 9 : compter les emprunts.
#
# Copie la chaine d'echange du moteur (NkXrSwapchain.h + NkXrTypes.h) dans
# src\NKXR, compile la salle instrumentee, puis la lance deux fois :
#   1) une minute de boucle normale (les trois nombres de reponse.txt) ;
#   2) vingt secondes ou la trame 300 prend le chemin qui fuit.
# Les deux bilans vont dans ..\compteurs.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

param(
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

$xr = Join-Path $NkentseuSource "Kernel\Runtime\NKXR\src\NKXR"
New-Item -ItemType Directory -Force "src\NKXR" | Out-Null
Copy-Item (Join-Path $xr "NkXrTypes.h") "src\NKXR\NkXrTypes.h" -Force
(Get-Content (Join-Path $xr "NkXrSwapchain.h") -Raw).Replace('#include "NKXR/NkXrTypes.h"', '#include "NkXrTypes.h"') |
    Set-Content "src\NKXR\NkXrSwapchain.h" -Encoding UTF8

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleEmprunts.exe" |
    Where-Object { $_.FullName -match "Release" } | Select-Object -First 1

if (Test-Path "compteurs_sortie.txt") { Remove-Item "compteurs_sortie.txt" }

Write-Host "1) Une minute de boucle normale..." -ForegroundColor Green
$env:MASALLE_DUREE = "60"
$env:MASALLE_FUITE = "0"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait | Out-Null

Write-Host "2) Vingt secondes, la trame 300 prend le chemin qui fuit (l'image se fige)..." -ForegroundColor Green
$env:MASALLE_DUREE = "20"
$env:MASALLE_FUITE = "300"
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait | Out-Null
Remove-Item Env:\MASALLE_DUREE
Remove-Item Env:\MASALLE_FUITE

if (-not (Test-Path "compteurs_sortie.txt")) {
    Write-Host "Rien d'ecrit. Fin du journal du moteur :" -ForegroundColor Red
    if (Test-Path "logs\app.log") { Get-Content "logs\app.log" -Tail 30 }
    exit 1
}
Get-Content "compteurs_sortie.txt" | Set-Content (Join-Path ".." "compteurs.txt") -Encoding ASCII
Write-Host ""
Get-Content "compteurs_sortie.txt"
