# mesurer.ps1 — chapitre 06, exercice 7 : le cout mesure.
#
# Compile ..\cout.cpp avec ..\MaSalle.jenga en Debug puis en Release, lance
# chaque fois la mesure (sans ouvrir la salle) et ecrit les douze lignes
# dans ..\mesures.txt.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1

$ErrorActionPreference = "Stop"
Set-Location $PSScriptRoot

# cout.cpp et MaSalle.jenga vivent dans le dossier de l'exercice (..\) :
# on en copie la version a jour ici avant chaque construction.
New-Item -ItemType Directory -Force "src" | Out-Null
Copy-Item (Join-Path ".." "cout.cpp") "src\cout.cpp" -Force
Copy-Item (Join-Path ".." "MaSalle.jenga") "MaSalle.jenga" -Force

$toutes = @()
foreach ($config in @("Debug", "Release")) {
    Write-Host "`n=== $config ===" -ForegroundColor Cyan
    jenga build --config $config
    if ($LASTEXITCODE -ne 0) {
        Write-Host "La compilation $config a echoue." -ForegroundColor Red
        exit 1
    }
    $exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalle.exe" |
        Where-Object { $_.FullName -match $config } |
        Select-Object -First 1

    if (Test-Path "cout_sortie.txt") { Remove-Item "cout_sortie.txt" }
    $env:MASALLE_CONFIG = $config
    $env:MASALLE_MESURE_SEULE = "1"
    Write-Host "Mesure en cours (quelques secondes)..."
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    if (-not (Test-Path "cout_sortie.txt")) {
        Write-Host "Aucune mesure ecrite en $config." -ForegroundColor Red
        exit 1
    }
    $sortie = Get-Content "cout_sortie.txt"
    $sortie | ForEach-Object { Write-Host $_ }
    $toutes += $sortie | Where-Object { $_ -match "^$config, " }
}
Remove-Item Env:MASALLE_CONFIG, Env:MASALLE_MESURE_SEULE

$toutes | Set-Content (Join-Path ".." "mesures.txt") -Encoding ASCII
Write-Host "`nLes douze mesures sont dans ..\mesures.txt" -ForegroundColor Green
