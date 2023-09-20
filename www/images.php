<?php
// Author: Peter Jensen

$swimDir = './swims';

function getImages($rootDir, $group, $subgroup) {
  $matchRe = '/([0-9]{8})\.jpg/';
  $dirName = $rootDir . '/' . sprintf("%04d", $group) . '/' . sprintf("%04d", $subgroup);
  $images = array();
  $dir = @opendir($dirName);
  if ($dir == false)
    return $images;
  while (false !== ($imageName = readdir($dir))) {
    if (preg_match($matchRe, $imageName, $matches)) {
      $images[] = intval($matches[1]);
    }
  }
  closedir($dir);
  sort($images);
  return $images;
}

function imagesJson() {
  global $swimDir;
  global $argv;
  if (php_sapi_name() == 'cli') {
    if (count($argv) < 3) {
      echo "ERROR: Two arguments expected\n";
      return;
    }
    $group = $argv[1];
    $subgroup = $argv[2];
    if (count($argv) > 3)
      $dir = $argv[3];
    else
      $dir = $swimDir;
  }
  else {
    $group = $_GET['group'];
    $subgroup = $_GET['subgroup'];
    if (isset($_GET['dir']))
      $dir = $_GET['dir'];
    else
      $dir = $swimDir;
  }
  echo json_encode(getImages($dir, $group, $subgroup));
}

function allImages() {
  echo "const allImages = [\n";
  $sg = 0;
  while (true) {
    $images = getImages("./downloads", 3, $sg);
    if (count($images) > 0) {
      echo json_encode($images);
      echo ",\n";
    }
    else
      break;
    $sg += 1;
  }
  echo "];";
}

function main() {
  imagesJson();
  //allImages();
}

main();
?>