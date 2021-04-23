

<!DOCTYPE html>
<html>
<body>

<?php
echo "<table style='border: solid 1px black;'>";
echo "<table><tr> <th>N</th> <th>Odd</th> <th>j</th>  <th>m</th>  <th>mm</th> <th>pi_N</th> 
<th>TT_Nj</th> <th>ppi_Nj</th> <th>qq_Nj</th> <th>ZZ_Nj</th> <th>FF_Nj</th>  <th>VV_Nj</th>  <th>LL_Nj</th> <th>PP_Nj</th>
<th>TT#</th>  </tr>";

 
class TableRows extends RecursiveIteratorIterator {
    function __construct($it) {
        parent::__construct($it, self::LEAVES_ONLY);
    }

    function current() {
        return "<td style='width: 150px; border: 1px solid black;'>" . parent::current() . "</td>";
    }

    function beginChildren() {
        echo "<tr>";
    }

    function endChildren() {
        echo "</tr>" . "\n";
    }
}

$servername = "localhost";
$username = "id16232074_main";
$password = "CoSemJenomDat17!";
$dbname = "id16232074_prime17";

try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    $stmt = $conn->prepare("select * from v_TT");
    $stmt->execute();

    // set the resulting array to associative
    $result = $stmt->setFetchMode(PDO::FETCH_ASSOC);

    foreach(new TableRows(new RecursiveArrayIterator($stmt->fetchAll())) as $k=>$v) {
        echo $v;
    }
}
catch(PDOException $e) {
    echo "Error: " . $e->getMessage();
}
$conn = null;
echo "</table>";
?>
Slices T full outputs.

</body>
</html>