<html>
<body>

<?php
// define variables and set to empty values
$worker_id =  "";

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
  $sql = "INSERT INTO test (teststr) VALUES ('$$worker_id')";
  //echo "SQL: " & $sql;
  echo $sql;
  // use exec() because no results are returned
  $conn->exec($sql);
  echo "New record created successfully";
} catch(PDOException $e) {
  echo $sql . "<br>" . $e->getMessage();
}

$conn = null;

?>    
    
    

Power of Two is: <?php echo $power; ?><br>
Your email address is: <?php echo $email; ?>





</body>
</html>