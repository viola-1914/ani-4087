# mesurer.ps1 — chapitre 08, exercice 10 : la chaine a trois images.
#
# Copie la chaine d'echange du moteur (NkXrSwapchain.h + NkXrTypes.h) dans
# src\NKXR, compile la salle, puis la lance deux fois :
#   - avec 3 images  -> ..\trente-lignes.txt (les 30 premieres trames)
#   - avec 1 image   -> pour comparer la duree des trames
# Les deux mesures vont dans ..\mesure.txt.
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

# La chaine du moteur, a cote du programme (include relatif, sans chemin a ajouter).
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

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleTrois.exe" |
    Where-Object { $_.FullName -match "Release" } | Select-Object -First 1

if (Test-Path "mesure_sortie.txt") { Remove-Item "mesure_sortie.txt" }

foreach ($n in @(1, 3)) {
    if (Test-Path "chaine_sortie.txt") { Remove-Item "chaine_sortie.txt" }
    Write-Host "La salle tourne avec $n image(s) (quelques secondes)..." -ForegroundColor Green
    $env:MASALLE_IMAGES = "$n"
    $p = Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait -PassThru
    if (-not (Test-Path "chaine_sortie.txt")) {
        Write-Host "Rien d'ecrit (code $($p.ExitCode)). Fin du journal du moteur :" -ForegroundColor Red
        if (Test-Path "logs\app.log") { Get-Content "logs\app.log" -Tail 30 }
        exit 1
    }
}
Remove-Item Env:\MASALLE_IMAGES

Get-Content "chaine_sortie.txt" | Set-Content (Join-Path ".." "trente-lignes.txt") -Encoding ASCII
Get-Content "mesure_sortie.txt" | Set-Content (Join-Path ".." "mesure.txt") -Encoding ASCII

Write-Host "`nLes trente lignes (3 images) :" -ForegroundColor Green
Get-Content "chaine_sortie.txt"
Write-Host "`nMesure :" -ForegroundColor Green
Get-Content "mesure_sortie.txt"
