# tester.ps1 — chapitre 06, exercice 4 : les crochets de test.
#
# Lance le test de la tete deux fois, SANS fenetre (MASALLE_TEST=1) :
#   1) tel quel ;
#   2) avec le signe du lacet inverse dans Regard() (INVERSER_LACET 1).
# Ecrit les lignes des deux lancements, chacune suivie de son code de
# sortie, dans ..\test.txt, puis reconstruit la version correcte.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\tester.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

$source = Join-Path ".." "tete.cpp"
$texte = Get-Content $source -Raw
if ($texte -notmatch "#define INVERSER_LACET 0") {
    Write-Host "tete.cpp doit contenir '#define INVERSER_LACET 0'." -ForegroundColor Red
    exit 1
}

function Lancer([string]$version, [string]$titre) {
    # Le code est copie dans src\ avec la version demandee, puis construit.
    $utf8 = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText((Join-Path $PSScriptRoot "src\tete.cpp"), $version, $utf8)
    jenga build --config Release | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "La compilation a echoue ($titre). Relancez 'jenga build --config Release' pour voir l'erreur." -ForegroundColor Red
        exit 1
    }
    $exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleTete.exe" |
        Where-Object { $_.FullName -match "Release" } |
        Select-Object -First 1

    if (Test-Path "test_sortie.txt") {
        Remove-Item "test_sortie.txt"
    }
    $env:MASALLE_TEST = "1"
    $processus = Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait -PassThru
    Remove-Item Env:\MASALLE_TEST

    $lignes = @()
    if (Test-Path "test_sortie.txt") {
        $lignes = @(Get-Content "test_sortie.txt")
    }
    $lignes += "code de sortie : $($processus.ExitCode)"

    Write-Host "`n$titre" -ForegroundColor Green
    $lignes | ForEach-Object { Write-Host $_ }
    return ,$lignes
}

$correct = Lancer $texte "Lancement 1 : Regard() tel quel"
$inverse = Lancer ($texte -replace "#define INVERSER_LACET 0", "#define INVERSER_LACET 1") "Lancement 2 : signe du lacet inverse dans Regard()"

# On remet le bon signe avant de finir.
Lancer $texte "Retour au bon signe (non recopie dans test.txt)" | Out-Null

$contenu = @("Lancement 1 : Regard() tel quel") + $correct + @("", "Lancement 2 : signe du lacet inverse dans Regard()") + $inverse
$utf8 = New-Object System.Text.UTF8Encoding($false)
[System.IO.File]::WriteAllText((Join-Path $PSScriptRoot "..\test.txt"), (($contenu -join "`n") + "`n"), $utf8)
Write-Host "`n..\test.txt ecrit." -ForegroundColor Green
