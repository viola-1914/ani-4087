# mesurer.ps1 - chapitre 07, exercice 4 : le chemin long provoque.
#
# 1. Compile ..\chemin.cpp TEL QUEL (sans le test du chemin court), lance le
#    tour programme a 20 deg/s (un tour complet, environ 20 secondes, la salle
#    se ferme seule) et garde toutes les lignes dans chemin_sans_test.txt.
# 2. Recompile une copie ou le test est remis (dans src\ seulement : ..\chemin.cpp
#    n'est pas modifie), relance, garde tout dans chemin_avec_test.txt.
# 3. Trouve l'image fautive (delta.w negatif) et ecrit ..\chemin.txt :
#    "sans le test" + l'image d'avant, la fautive, l'image d'apres ;
#    "avec le test" + les trois lignes autour du meme lacet.
#
# Lancement, depuis ce dossier, dans PowerShell :
#     powershell -ExecutionPolicy Bypass -File .\mesurer.ps1
# Ne touchez a rien pendant les deux tours.

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

$marqueur = "    // (test du chemin court retire)"
$test = "    if (delta.w < 0.f) {`n        delta = NkQuatf(-delta.x, -delta.y, -delta.z, -delta.w);`n    }"

function Tour([bool]$avecTest, [string]$fichier) {
    $code = Get-Content (Join-Path ".." "chemin.cpp") -Raw
    if ($avecTest) {
        if (-not $code.Contains($marqueur)) {
            Write-Host "Marqueur introuvable dans chemin.cpp" -ForegroundColor Red
            exit 1
        }
        $code = $code.Replace($marqueur, $test)
    }
    Set-Content "src\chemin.cpp" $code -Encoding ASCII -NoNewline
    Write-Host "Compilation ($fichier)..."
    jenga build --config Release | Out-Null
    if ($LASTEXITCODE -ne 0) {
        Write-Host "La compilation a echoue." -ForegroundColor Red
        jenga build --config Release
        exit 1
    }
    $exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleChemin.exe" |
        Where-Object { $_.FullName -match "Release" } |
        Select-Object -First 1
    if (Test-Path "chemin_sortie.txt") { Remove-Item "chemin_sortie.txt" }
    $env:MASALLE_TOUR = "20"
    Write-Host "Tour a 20 deg/s (environ 20 secondes, ne touchez a rien)..." -ForegroundColor Green
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    Remove-Item Env:MASALLE_TOUR -ErrorAction SilentlyContinue
    if (-not (Test-Path "chemin_sortie.txt")) {
        Write-Host "  aucune sortie" -ForegroundColor Red
        exit 1
    }
    Copy-Item "chemin_sortie.txt" $fichier -Force
    return @(Get-Content $fichier | Where-Object { $_ -match "^image " })
}

function Lacet([string]$ligne) {
    if ($ligne -match "lacet = (-?[0-9.]+) deg") { return [double]$Matches[1] }
    return [double]::NaN
}

$sans = Tour $false "chemin_sans_test.txt"
$avec = Tour $true "chemin_avec_test.txt"

# L'image fautive : delta.w negatif.
$i = -1
for ($k = 0; $k -lt $sans.Count; $k++) {
    if ($sans[$k] -match "delta.w = -") { $i = $k; break }
}
if ($i -lt 1) {
    Write-Host "Aucune image avec delta.w negatif : regardez chemin_sans_test.txt" -ForegroundColor Red
    exit 1
}
$lacetFautif = Lacet $sans[$i]

# Avec le test : la ligne dont le lacet est le plus proche.
$j = 0
$meilleur = [double]::MaxValue
for ($k = 0; $k -lt $avec.Count; $k++) {
    $e = [math]::Abs((Lacet $avec[$k]) - $lacetFautif)
    if ($e -lt $meilleur) { $meilleur = $e; $j = $k }
}
$j = [math]::Max(1, [math]::Min($j, $avec.Count - 2))

$lignes = @("sans le test") + $sans[($i - 1)..($i + 1)] + @("avec le test") + $avec[($j - 1)..($j + 1)]
$lignes | Set-Content (Join-Path ".." "chemin.txt") -Encoding ASCII

# On remet src\ comme ..\chemin.cpp (sans le test).
Copy-Item (Join-Path ".." "chemin.cpp") "src\chemin.cpp" -Force

Write-Host "`nchemin.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "chemin.txt")
$nbNeg = @($sans | Where-Object { $_ -match "delta.w = -" }).Count
Write-Host "`n(info) images sans le test : $($sans.Count), dont $nbNeg avec delta.w negatif ; images avec le test : $($avec.Count)"
