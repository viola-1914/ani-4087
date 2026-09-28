# mesure-blocage.ps1 — exercice 2, chapitre 03.
#
# Lance MaFenetre.exe, attend que sa fenetre apparaisse (T0), puis interroge le
# processus jusqu'a ce que Windows le declare non reactif. Mesure aussi la part
# de processeur consommee par la boucle vide.
#
#   powershell -ExecutionPolicy Bypass -File mesure-blocage.ps1
#
# La fenetre reste ouverte a la fin : faites la capture, puis fermez-la.

$exe = "Build\Bin\Debug-Windows\MaFenetre\MaFenetre.exe"

if (-not (Test-Path $exe)) {
    Write-Host "Introuvable : $exe  --  lancez d'abord : jenga build --config Debug"
    exit 1
}

$p = Start-Process $exe -PassThru
Write-Host "Processus lance (PID $($p.Id)), attente de la fenetre..."

while ($p.MainWindowHandle -eq 0 -and -not $p.HasExited) {
    Start-Sleep -Milliseconds 20
    $p.Refresh()
}

$t0 = Get-Date
Write-Host ("T0 - fenetre ouverte a {0}" -f $t0.ToString("HH:mm:ss.fff"))

$cpu0 = $p.TotalProcessorTime

while (-not $p.HasExited) {
    $p.Refresh()
    if (-not $p.Responding) { break }
    Start-Sleep -Milliseconds 100
}

$delai = (Get-Date) - $t0
Write-Host ""
Write-Host ("DELAI : declaree bloquee apres {0:N2} secondes" -f $delai.TotalSeconds)

Start-Sleep -Seconds 3
$p.Refresh()
$cpu = ($p.TotalProcessorTime - $cpu0).TotalSeconds
$coeurs = [Environment]::ProcessorCount
Write-Host ("PROCESSEUR : {0:N2} s de calcul en {1:N2} s de temps reel" -f $cpu, ($delai.TotalSeconds + 3))
Write-Host ("             soit environ {0:N0} % d'un coeur ({1} coeurs sur cette machine)" -f (100 * $cpu / ($delai.TotalSeconds + 3)), $coeurs)
Write-Host ""
Write-Host "La fenetre est toujours la : faites vos captures."
Write-Host "Pour l'arreter :  taskkill /IM MaFenetre.exe /F"
