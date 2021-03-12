<html>
<body>

<?php
// remove any string that could create an invalid JSON 
// such as PHP Notice, Warning, logs...
//ob_clean();

// this will clean up any previously added headers, to start clean
//header_remove(); 
//header('Content-Type: application/json');

// define variables and set to empty values
$worker_id = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $worker_id = test_input($_POST["worker_id"]);
}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

$servername = "localhost";
$username = "id16232074_main";
$password = "CoSemJenomDat17!";
$dbname = "id16232074_prime17";

try {
  $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
  // set the PDO error mode to exception
  $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

//  $sql = "CALL get_work()";
//   $q = $conn->query($sql);
// Then you can prepare a statement and execute it.    
$q = $conn->prepare("CALL get_work(?)");
// One bindParam() call per parameter
$q->bindParam(1, $worker_id, PDO::PARAM_INT); 
$q ->execute();

$q->setFetchMode(PDO::FETCH_ASSOC);
} catch (PDOException $e) {
            die("Error occurred:" . $e->getMessage());
}
    $rows = array();
    while ($r = $q->fetch()):
        $rows[] = $r;
    endwhile;
$conn = null;


http_response_code(200);
echo "OUTPUT_BEGIN";
echo json_encode($rows);
echo "OUTPUT_END";
exit();

?>    

</body>
</html>