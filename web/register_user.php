<html>
<body>

<?php
$email = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
  $email = test_input($_POST["email"]);
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
$q = $conn->prepare("CALL register_user_get_workers(?)");
// One bindParam() call per parameter
$q->bindParam(1, $email, PDO::PARAM_STR); 
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