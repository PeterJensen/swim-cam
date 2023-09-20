<?php

class Config {
  public static $rootDir = "downloads/0003";
  public static $outDir  = "./";
  public static $maxTime = 10*60*1000;
}

function timeOf($file) {
  preg_match('/([0-9]{8})\.jpg/', $file, $matches);
  return intval($matches[1]);
}

function main() {
  $dh = opendir(Config::$rootDir);
  $subgroups = array();
  while (false !== ($entry = readdir($dh))) {
    if (preg_match('/[0-9]{4}/', $entry) == 1) {
      $subgroups[] = $entry;
    }
  }
  closedir($dh);
  sort($subgroups);
  $files = array();
  foreach ($subgroups as $sg) {
    $sgh = opendir(Config::$rootDir . '/' . $sg);
    while (false !== ($file = readdir($sgh))) {
      if (preg_match('/[0-9]{8}/', $file)) {
        $files[] = $sg . '/' . $file;
      }
    }
    closedir($sgh);
  }
  sort($files);

  $timeGroups = array();
  $timeGroup = array();
  $maxTime = Config::$maxTime;
  foreach ($files as $file) {
    if (timeOf($file) < $maxTime) {
      $timeGroup[] = $file;
    }
    else {
      $maxTime += Config::$maxTime;
      $timeGroups[] = $timeGroup;
      $timeGroup = array();
    }
  }
  if (count($timeGroup) > 0) {
    $timeGroups[] = $timeGroup;
  }

  $start = 0;  
  foreach ($timeGroups as $tg) {
    $end = $start + Config::$maxTime;
    $fname = sprintf("%08d-%08d.txt", $start, $end);
    $fh = fopen($fname, 'w');
    foreach ($tg as $name) {
      $fullName = Config::$rootDir . '/' . $name;
      fwrite($fh, "file '" . $fullName . "'\n");
    }
    fclose($fh);
    $start += Config::$maxTime;
  }

  return;


  for ($i = 0; $i < count($files); $i += 100) {
    echo "$files[$i]\n";
    $t = timeOf($files[$i]);
    echo "Time: $t\n";
  }
}

main();
?>