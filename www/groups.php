<?php
// Author: Peter Jensen

$swimDir = './swims';

function numSubgroups($groupDir) {
  $matchRe = '/[0-9]{4}/';
  $dir = opendir($groupDir);
  if ($dir === false)
    return 0;
  $subgroupCount = 0;
  while (false !== ($subgroupDir = readdir($dir))) {
    if (preg_match($matchRe, $subgroupDir))
      $subgroupCount += 1;
  }
  closedir($dir);
  return $subgroupCount;
}

function findGroups($rootDir) {
  $matchRe = '/[0-9]{4}/';
  $groups = array();
  $dir = opendir($rootDir);
  if ($dir === false)
    return $groups;
  while (false !== ($groupDir = readdir($dir))) {
    if (preg_match($matchRe, $groupDir, $matches))
      $groups[] = numSubgroups($rootDir . '/' . $groupDir);
  }
  closedir($dir);
  return $groups;
}

function main() {
  global $swimDir;
  echo json_encode(findGroups($swimDir));
}

main();
?>