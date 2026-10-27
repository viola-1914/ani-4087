# mesurer.ps1 - chapitre 07, exercice 3 : mesurer une vitesse.
#
# Compile la salle avec ..\vitesse.cpp, puis la lance deux fois :
#   1. MASALLE_TOUR=30 : le lacet suit l'horloge a 30 deg/s -> "serie programme"
#      (ne touchez a rien ; la salle se ferme seule)
#   2. a la souris -> "serie souris" : quand la salle est ouverte, deplacez la
#      souris vers la droite, d'un geste regulier, a vitesse a peu pres constante.
#      L'enregistrement commence au premier mouvement ; la salle se ferme seule
#      apres vingt images.
# Ecrit ..\vitesses.txt avec les deux series.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

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

New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "vitesse.cpp") "src\vitesse.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleVitesse.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

function Lancer([string]$fichier) {
    if (Test-Path "vitesse_sortie.txt") { Remove-Item "vitesse_sortie.txt" }
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    if (-not (Test-Path "vitesse_sortie.txt")) {
        Write-Host "  aucune sortie" -ForegroundColor Red
        exit 1
    }
    Copy-Item "vitesse_sortie.txt" $fichier -Force
    return Get-Content "vitesse_sortie.txt"
}

Write-Host "`n1. Tour programme a 30 deg/s : ne touchez a rien..." -ForegroundColor Green
$env:MASALLE_TOUR = "30"
$programme = Lancer "vitesse_programme.txt"
Remove-Item Env:MASALLE_TOUR -ErrorAction SilentlyContinue

Write-Host "`n2. A la souris : quand la salle s'ouvre, deplacez la souris vers la droite," -ForegroundColor Green
Write-Host "   d'un geste regulier, a vitesse a peu pres constante, pendant 2 secondes." -ForegroundColor Green
Read-Host "   Appuyez sur Entree pour ouvrir la salle"
$souris = Lancer "vitesse_souris.txt"

$garder = { param($l) $l -match "^serie " -or $l -match "^image " }
@($programme | Where-Object { & $garder $_ }) + @($souris | Where-Object { & $garder $_ }) |
    Set-Content (Join-Path ".." "vitesses.txt") -Encoding ASCII

Write-Host "`nvitesses.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "vitesses.txt")
