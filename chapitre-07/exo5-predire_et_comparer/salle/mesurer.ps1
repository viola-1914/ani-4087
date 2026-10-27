# mesurer.ps1 - chapitre 07, exercice 5 : predire et comparer.
#
# Compile la salle avec ..\prediction.cpp et la lance avec MASALLE_HZ = la
# frequence de l'ecran (60 Hz dans les parametres d'affichage de Windows).
# Quand la salle est ouverte, regardez autour de vous a la souris, comme on
# regarde autour de soi : tourner, s'arreter, repartir, lever et baisser les
# yeux. Apres mille images (environ 20 secondes), la salle se ferme seule.
# Ecrit ..\erreurs.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

param(
    # Frequence de l'ecran, lue dans les parametres d'affichage de Windows.
    [string]$Hz = "60",
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

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "prediction.cpp") "src\prediction.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSallePrediction.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Write-Host "`nQuand la salle s'ouvre : regardez autour de vous a la souris pendant" -ForegroundColor Green
Write-Host "environ 20 secondes (tourner, s'arreter, repartir, lever et baisser les yeux)." -ForegroundColor Green
Read-Host "Appuyez sur Entree pour ouvrir la salle"

if (Test-Path "prediction_sortie.txt") { Remove-Item "prediction_sortie.txt" }
$env:MASALLE_HZ = $Hz
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
Remove-Item Env:MASALLE_HZ -ErrorAction SilentlyContinue

if (-not (Test-Path "prediction_sortie.txt")) {
    Write-Host "Aucune sortie." -ForegroundColor Red
    exit 1
}
$lignes = Get-Content "prediction_sortie.txt"
$lignes | Where-Object { $_ -match "^(avec|sans) prediction : " } | Set-Content (Join-Path ".." "erreurs.txt") -Encoding ASCII

Write-Host "`nerreurs.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "erreurs.txt")
$lignes | Where-Object { $_ -match "^\(info\)" }
