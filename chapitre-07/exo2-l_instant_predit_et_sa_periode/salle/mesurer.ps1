# mesurer.ps1 - chapitre 07, exercice 2 : l'instant predit et sa periode.
#
# Compile la salle avec ..\instant.cpp, puis la lance quatre fois, 700 images
# chacune (elle se ferme seule), avec MASALLE_HZ = 72, 60, 90 et 120.
# Ecrit ..\instants.txt : les vingt lignes a 72 Hz, puis les quatre cadences.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1
# Pendant les mesures, ne touchez pas la souris ni le clavier.

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
Copy-Item (Join-Path ".." "instant.cpp") "src\instant.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleInstant.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

$vingt = @()
$cadences = @()
$infos = @()
foreach ($hz in @("72", "60", "90", "120")) {
    if (Test-Path "instant_sortie.txt") { Remove-Item "instant_sortie.txt" }
    $env:MASALLE_HZ = $hz
    $env:MASALLE_EXIT = "700"
    Write-Host "Mesure a $hz Hz (environ 12 secondes, ne touchez a rien)..." -ForegroundColor Green
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    Remove-Item Env:MASALLE_HZ, Env:MASALLE_EXIT -ErrorAction SilentlyContinue
    if (-not (Test-Path "instant_sortie.txt")) {
        Write-Host "  aucune sortie" -ForegroundColor Red
        exit 1
    }
    $lignes = Get-Content "instant_sortie.txt"
    Copy-Item "instant_sortie.txt" "instant_$hz.txt" -Force
    if ($hz -eq "72") { $vingt = $lignes | Where-Object { $_ -match "^maintenant = " } }
    $cadences += $lignes | Where-Object { $_ -match "^hz = " }
    $infos += $lignes | Where-Object { $_ -match "^\(info\)" } | ForEach-Object { "$hz Hz : $_" }
}

@($vingt) + @($cadences) | Set-Content (Join-Path ".." "instants.txt") -Encoding ASCII

Write-Host "`ninstants.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "instants.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$infos
