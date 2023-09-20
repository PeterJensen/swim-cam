<?php
// Author: Peter Jensen

$swimDir = './swims';

function getImageName($rootDir, $group, $subgroup, $timestamp) {
  return sprintf('%s/%04u/%04u/%08u.jpg', $rootDir, intval($group), intval($subgroup), intval($timestamp));
}

function main() {
  global $swimDir;
  global $argv;
  if (php_sapi_name() == 'cli') {
    if (count($argv) < 4) {
      echo "ERROR: Three arguments expected\n";
      return;
    }
    $group = $argv[1];
    $subgroup = $argv[2];
    $timestamp = $argv[3];
    $dir = $argv[4];
  }
  else {
    $group = $_GET['group'];
    $subgroup = $_GET['subgroup'];
    $timestamp = $_GET['timestamp'];
    if (isset($_GET['dir']))
      $dir = $_GET['dir'];
    else
      $dir = $swimDir;
  }
  header('Content-Type: image/jpeg');
  readfile(getImageName($dir, $group, $subgroup, $timestamp));
}

main();
?>