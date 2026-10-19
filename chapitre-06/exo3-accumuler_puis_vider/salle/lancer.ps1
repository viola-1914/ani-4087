# lancer.ps1 — chapitre 06, exercice 2 : accumuler puis vider.
#
# Compile la salle avec ..\accumulation.cpp, l'ouvre, puis affiche les releves.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1
#
# Dans la salle : bougez la souris dans un seul sens pendant une seconde, puis
# lachez-la et n'y touchez plus pendant dix secondes. Echap ferme la salle.

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

# accumulation.cpp vit dans le dossier de l'exercice (..\) : on en copie la
# version a jour dans src\ avant chaque construction.
Copy-Item (Join-Path ".." "accumulation.cpp") "src\accumulation.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleAccumulation.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Write-Host "Ouverture de la salle : bougez la souris dans UN SEUL sens 1 s, puis n'y touchez plus 10 s. Echap ferme." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait

Write-Host "`nReleves (trois_releves.txt) :" -ForegroundColor Green
if (Test-Path "trois_releves.txt") {
    Get-Content "trois_releves.txt"
} else {
    Write-Host "Aucun releve : la salle n'a pas tourne une seconde entiere." -ForegroundColor Red
}
