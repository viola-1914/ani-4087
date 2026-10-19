# lancer.ps1 — chapitre 06, exercice 6 : le cone du mauvais ordre.
#
# 1. Compile la salle avec ..\cone.cpp
# 2. Prend les trois captures (pose figee, sans souris) :
#      bon-90.png     lacet*tangage, lacet 90
#      mauvais-0.png  tangage*lacet, lacet 0
#      mauvais-90.png tangage*lacet, lacet 90
# 3. Recopie les dix hauteurs imprimees par le programme dans ..\hauteurs.txt
# 4. Ouvre la salle pour le tour complet a la souris
#    (Espace : changer d'ordre, Echap : quitter).
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1

param(
    # Code source de Nkentseu : il contient Resources\NKRenderer (shaders).
    [string]$NkentseuSource = "C:\Users\HP\Documents\GitHub\Nkentseu",
    # OpenGL rend les captures la tete en bas : on les retourne. -SansRetourner pour ne pas le faire.
    [switch]$SansRetourner
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

# cone.cpp vit dans le dossier de l'exercice (..\) : on en copie la version
# a jour dans src\ avant chaque construction.
New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "cone.cpp") "src\cone.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleCone.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

Add-Type -AssemblyName System.Drawing

$captures = @(
    @{ Ordre = "bon";     Lacet = "90"; Fichier = "bon-90.png" },
    @{ Ordre = "mauvais"; Lacet = "0";  Fichier = "mauvais-0.png" },
    @{ Ordre = "mauvais"; Lacet = "90"; Fichier = "mauvais-90.png" }
)

$echecs = 0
foreach ($c in $captures) {
    if (Test-Path $c.Fichier) { Remove-Item $c.Fichier }
    Write-Host "Capture $($c.Fichier)..."
    $env:MASALLE_ORDRE = $c.Ordre
    $env:MASALLE_LACET = $c.Lacet
    $env:MASALLE_SHOT = "120"
    $env:MASALLE_SHOT_PATH = $c.Fichier
    $p = Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait -PassThru
    if ($p.ExitCode -ne 0 -or -not (Test-Path $c.Fichier)) {
        Write-Host "  -> echec (code $($p.ExitCode))" -ForegroundColor Red
        $echecs++
        continue
    }
    $destination = Join-Path (Resolve-Path "..") $c.Fichier
    if ($SansRetourner) {
        Copy-Item $c.Fichier $destination -Force
    } else {
        $img = [System.Drawing.Image]::FromFile((Resolve-Path $c.Fichier))
        $img.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipY)
        $img.Save($destination, [System.Drawing.Imaging.ImageFormat]::Png)
        $img.Dispose()
    }
    Remove-Item $c.Fichier
    Write-Host "  -> ..\$($c.Fichier)" -ForegroundColor Green
}
Remove-Item Env:MASALLE_ORDRE, Env:MASALLE_LACET, Env:MASALLE_SHOT, Env:MASALLE_SHOT_PATH

if (Test-Path "hauteurs_sortie.txt") {
    Copy-Item "hauteurs_sortie.txt" (Join-Path ".." "hauteurs.txt") -Force
    Write-Host "`nHauteurs (copiees dans ..\hauteurs.txt) :" -ForegroundColor Green
    Get-Content "hauteurs_sortie.txt"
} else {
    Write-Host "Aucune hauteur ecrite." -ForegroundColor Red
}

if ($echecs -gt 0) {
    Write-Host "`n$echecs capture(s) en echec." -ForegroundColor Red
}

Write-Host "`nOuverture de la salle : un tour complet lent dans chaque ordre." -ForegroundColor Green
Write-Host "Espace : changer d'ordre (le titre l'indique). Echap : quitter." -ForegroundColor Green
Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
