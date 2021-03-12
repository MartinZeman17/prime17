<html>
<body>

<?php

// define variables and set to empty values
$work="";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $work = test_input($_POST["work"]);
  //echo $work;
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

    // Then you can prepare a statement and execute it.    
    $q = $conn->prepare("CALL work_received(?)");
    // One bindParam() call per parameter
    $q->bindParam(1, $work, PDO::PARAM_STR, 20000); 
    $q ->execute();
    
    $q->setFetchMode(PDO::FETCH_ASSOC);
} catch (PDOException $e) {
            die("Error occurred:" . $e->getMessage());
}

echo "OUTPUT_BEGIN";
$rows = array();
while ($r = $q->fetch()):
    $rows[] = $r;
    // echo $r;
    echo $r['output'];
endwhile;
$conn = null;

echo "OUTPUT_END";
http_response_code(200);
exit();

?>    

</body>
</html>