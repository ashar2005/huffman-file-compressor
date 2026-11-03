$files = Get-ChildItem tests -File | Where-Object { $_.Extension -notin '.huf', '.out', '.ps1' }
$rows = foreach ($f in $files) {
    $huf = "$($f.FullName).huf"
    $out = "$($f.FullName).out"
    .\huffman.exe compress $f.FullName $huf | Out-Null
    .\huffman.exe decompress $huf $out | Out-Null
    $ok = (Get-FileHash $f.FullName).Hash -eq (Get-FileHash $out).Hash
    $o = $f.Length
    $c = (Get-Item $huf).Length
    [pscustomobject]@{
        File       = $f.Name
        Original   = $o
        Compressed = $c
        'Ratio %'  = if ($o -gt 0) { [math]::Round(100 * $c / $o, 1) } else { 'n/a' }
        RoundTrip  = if ($ok) { 'OK' } else { 'FAILED' }
    }
}
$rows | Format-Table -AutoSize