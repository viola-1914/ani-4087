# mesurer.ps1 — chapitre 07, exercice 1 : votre budget calcule.
#
# Compile la salle avec ..\budget.cpp, puis la lance deux fois, 1040 images
# chacune (elle se ferme seule) :
#   1. telle quelle : elle attend l'ecran avant chaque image  -> ligne "ecran"
#   2. avec NK_VK_MAILBOX=1 : sans cette attente (Vulkan)     -> ligne "sans attente"
# Ecrit ..\budget.txt (trois budgets + deux mesures) et affiche la frequence
# de l'ecran declaree par Windows.
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
Copy-Item (Join-Path ".." "budget.cpp") "src\budget.cpp" -Force

Write-Host "Compilation..."
jenga build --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "La compilation a echoue." -ForegroundColor Red
    exit 1
}

$exe = Get-ChildItem -Path "Build" -Recurse -Filter "MaSalleBudget.exe" |
    Where-Object { $_.FullName -match "Release" } |
    Select-Object -First 1

function Mesurer([string]$nom, [bool]$mailbox) {
    if (Test-Path "budget_sortie.txt") { Remove-Item "budget_sortie.txt" }
    $env:MASALLE_EXIT = "1040"
    if ($mailbox) { $env:NK_VK_MAILBOX = "1" } else { Remove-Item Env:NK_VK_MAILBOX -ErrorAction SilentlyContinue }
    Write-Host "Mesure '$nom' (environ 20 secondes, ne touchez a rien)..." -ForegroundColor Green
    Start-Process -FilePath $exe.FullName -WorkingDirectory $PSScriptRoot -Wait
    Remove-Item Env:MASALLE_EXIT, Env:NK_VK_MAILBOX -ErrorAction SilentlyContinue
    if (-not (Test-Path "budget_sortie.txt")) {
        Write-Host "  aucune sortie" -ForegroundColor Red
        exit 1
    }
    $lignes = Get-Content "budget_sortie.txt"
    Copy-Item "budget_sortie.txt" "budget_$($nom -replace ' ', '_').txt" -Force
    return $lignes
}

$ecran = Mesurer "ecran" $false
$sansAttente = Mesurer "sans attente" $true

# budget.txt : les trois budgets, puis les deux mesures.
$budgets = $ecran | Where-Object { $_ -match "^budget " }
$mesure1 = $ecran | Where-Object { $_ -match "^ecran : " }
$mesure2 = $sansAttente | Where-Object { $_ -match "^sans attente : " }
@($budgets) + @($mesure1) + @($mesure2) | Set-Content (Join-Path ".." "budget.txt") -Encoding ASCII

Write-Host "`nbudget.txt :" -ForegroundColor Green
Get-Content (Join-Path ".." "budget.txt")
Write-Host "`nDetails :" -ForegroundColor Green
$ecran | Where-Object { $_ -match "^\(info\)" } | ForEach-Object { "ecran        $_" }
$sansAttente | Where-Object { $_ -match "^\(info\)" } | ForEach-Object { "sans attente $_" }

Write-Host "`nFrequence declaree par Windows :" -ForegroundColor Green
Get-CimInstance Win32_VideoController | Select-Object Name, CurrentRefreshRate, CurrentHorizontalResolution, CurrentVerticalResolution | Format-Table -AutoSize
