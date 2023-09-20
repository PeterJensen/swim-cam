<?php

// Author: Peter Jensen

class Config {
  const topUrl    = "http://192.168.4.1";
//  const topUrl    = "http://localhost:8080";
  const groupsUrl = self::topUrl . "/groups";
  const imagesUrl = self::topUrl . "/images";
  const imageUrl  = self::topUrl . "/image";
  //const imagesUrl = self::topUrl . "/images.php";
  //const imageUrl = self::topUrl . "/image.php";
  const useCurl   = true;
  const saveDir   = "./downloads";
}

function fileGet($url) {
  $opts = array(
    'http' => array(
      'timeout' => 2,
      'method' => "GET",
      'header' => "Accept: */*\r\n"
    )
  );
  $context = stream_context_create($opts);
  $contents = file_get_contents($url, false, $context);
  return $contents;
}

function curlGet($url) {
  $curlSession = curl_init();
  curl_setopt($curlSession, CURLOPT_URL, $url);
  curl_setopt($curlSession, CURLOPT_RETURNTRANSFER, true);
  //curl_setopt($curlSession, CURLINFO_HEADER_OUT, true); // enable tracking
  $contents = curl_exec($curlSession);
  //$headers = curl_getinfo($curlSession, CURLINFO_HEADER_OUT ); // request headers
  curl_close($curlSession);
  return $contents;
}

function getFileContents($url) {
  if (Config::useCurl)
    return curlGet($url);
  else
    return fileGet($url);
}

function getFileJson($url) {
  return json_decode(getFileContents($url));
}

class Params {
  public static $group = 3;
}

function makeDir($dir) {
  if (!file_exists($dir))
    mkdir($dir);
}
function groupDir($groupId) {
  return sprintf("%04u", $groupId);
}
function fileName($timestamp) {
  return sprintf("%08u.jpg", $timestamp);
}
function writeFile($filename, $data) {
  $fh = fopen($filename, 'w');
  fwrite($fh, $data);
  fclose($fh);
}

function main() {
  $g = Params::$group;
  makeDir(Config::saveDir);
  $groupDir = Config::saveDir . "/" . groupDir($g);
  makeDir($groupDir);
  $sg = 66;
  while (true) {
    $imagesUrl = Config::imagesUrl . "?group=$g&subgroup=$sg";
    $timestamps = getFileJson($imagesUrl);
    if (count($timestamps) == 0)
      break;
    $subgroupDir = $groupDir . "/" . groupDir($sg);
    makeDir($subgroupDir);
    for ($i = 0; $i < count($timestamps); $i++) {
      $ts = $timestamps[$i];
      $imgUrl = Config::imageUrl . "?group=$g&subgroup=$sg&timestamp=$ts";
      $imgData = getFileContents($imgUrl);
      //echo "$imgUrl\n";
      $filename = $subgroupDir . "/" . fileName($ts);
      echo "Saving: $filename\n";
      writeFile($filename, $imgData);
    }
    $sg += 1;
  }
}

main();
?>
