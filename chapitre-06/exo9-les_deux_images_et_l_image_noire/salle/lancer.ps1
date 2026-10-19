# lancer.ps1 — chapitre 06, exercice 9 : les deux images et l'image noire.
#
# Compile la salle avec ..\deux-yeux.cpp et la lance : la fenetre montre les
# deux yeux cote a cote, capture a l'image 30 (yeux decales) puis a l'image 60
# (yeux oublies), et se ferme seule. Le programme soustrait ensuite les images.
# Le script recopie dans le dossier de l'exercice (..\) :
#   oeil-gauche.png, oeil-droit.png, difference.png,
#   difference-vue-oubliee.png, difference.txt
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1
# Si les images sortent la tete en bas :
#     powershell -ExecutionPolicy Bypass -File .\lancer.ps1 -Retourner

param(
    # Code source de Nkentseu : il contient Resources\NKRenderer (shaders).
    [string]$NkentseuSource = "C:\Users\HP\Documents\GitHub\Nkentseu",
    # Retourne les quatre images (haut-bas) en les recopiant.
    [switch]$Retourner
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
Copy-Item (Join-Path ".." "deux-yeux.cpp") "src\deux-yeux.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleDeuxYeux.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

$fichiers = @("oeil-gauche.png", "oeil-droit.png", "difference.png", "difference-vue-oubliee.png",
              "oeil-gauche-vue-oubliee.png", "oeil-droit-vue-oubliee.png",
              "difference_sortie.txt", "temoin_captures.txt")
foreach ($f in $fichiers) { if (Test-Path $f) { Remove-Item $f } }

Write-Host "La salle s'ouvre puis se ferme seule (quelques secondes)..." -ForegroundColor Green
$p = Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait -PassThru
Write-Host "Code de sortie : $($p.ExitCode)"

if (-not (Test-Path "difference_sortie.txt")) {
    Write-Host "Pas de difference ecrite. Fin du journal du moteur :" -ForegroundColor Red
    if (Test-Path "logs\app.log") { Get-Content "logs\app.log" -Tail 30 }
    exit 1
}

Add-Type -AssemblyName System.Drawing
foreach ($f in @("oeil-gauche.png", "oeil-droit.png", "difference.png", "difference-vue-oubliee.png")) {
    $destination = Join-Path (Resolve-Path "..") $f
    if ($Retourner) {
        $img = [System.Drawing.Image]::FromFile((Resolve-Path $f))
        $img.RotateFlip([System.Drawing.RotateFlipType]::RotateNoneFlipY)
        $img.Save($destination, [System.Drawing.Imaging.ImageFormat]::Png)
        $img.Dispose()
    } else {
        Copy-Item $f $destination -Force
    }
}
Get-Content "difference_sortie.txt" | Set-Content (Join-Path ".." "difference.txt") -Encoding ASCII

Write-Host "`ndifference.txt :" -ForegroundColor Green
Get-Content "difference_sortie.txt"
Write-Host "`nTemoin des captures (une image uniforme ne prouve rien) :" -ForegroundColor Green
Get-Content "temoin_captures.txt"
Invoke-Item (Join-Path ".." "difference.png")
