<?php

function filter($lines, $sg) {
  $sgs = sprintf("%04d", $sg);
  $fsg = array_values(array_filter($lines, function ($k) use ($sgs) {
    return substr($k, 0, 4) == $sgs;
  }));
  $tsa = array_map(function ($v) {return intval(substr($v, 5, 8));}, $fsg);
  return json_Encode($tsa);
}

function imagesJson($lines) {
  $images = "[\n";
  for ($g = 0; $g < 153; $g++)
    $images .= filter($lines, $g) . ",\n";
  $images .= "];\n";
  return $images;
}

function copyFiles($lines, $fromDir, $toDir, $usePath = true) {
  $fromSubdir = $fromDir . "/0003";
  $toSubdir = $toDir . "/0003";
  if (!file_exists($toDir))
    mkdir($toDir);
  if ($usePath && !file_exists($toSubdir))
    mkdir($toSubdir);
  foreach ($lines as $f) {
    if ($f == "")
      continue;
    $toFiledir = $toSubdir . "/" . substr($f, 0, 4);
    if ($usePath)
      $toFile = $toSubdir . "/" . $f;
    else {
      preg_match("/([0-9]{8}\.jpg)/", $f, $matches);
      $toFile = $toDir . "/" . $matches[1];
    }
    $fromFile = $fromSubdir . "/" . $f;
    if ($usePath && !file_exists($toFiledir)) {
      mkdir($toFiledir);
    }
    if (!file_exists($toFile))
      echo "cp $fromFile $toFile\n";
  }
}

function main() {
  global $argv;
  $infile = $argv[1];
  $contents = file_get_contents($infile);
  $lines = preg_split("/\n/", $contents);
//  $goodImages = "const bestImages = " . imagesJson($lines);
//  print($goodImages);
//  copyFiles($lines, "./downloads", "./good");
  copyFiles($lines, "./downloads", "./large", false);
}

main();
?>