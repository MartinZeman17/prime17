<!DOCTYPE html>
<html>
<body>

<?php
echo "<table style='border: solid 1px black;'>";
echo "<table><tr> <th>Interval</th> <th>Fully Completed</th> <th>Completed (gaps)</th>  <th>Taken</th> <th>Fully Completed %</th> <th>Completed (gaps) %</th>  <th>Taken %</th> <th>Start</th> <th>Last update</th> <th>duration</th> <th>Days</th> <th>Remaining Days</th></tr>";

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
    $stmt = $conn->prepare("CALL completed_report;");
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
</br>

This is a report on the overall progress of the computation. Each row takes slightly more than double the effort required for the previous interval. Well, the length of each interval is exactly twice the length of the previous one. However primality test for highger numbers gradually requires slightly more and more computational power so the effort is more than doubled.</br></br>

Legend:</br>
Fully Completed: Continuous completed range.</br>
Completed (gaps): Maximum of all completed nibbles. There may be lower unfinished parts which are taken or expired and waiting for a worker.</br>
Taken:  Maximum of all nibbles. May also include nibbles where computatiton is still in progress or expired.</br> 
</br> 
Start: Date and time when the computation started.</br> 
Last update: Date and time of the last succesfully delivered nibble.</br></br>  
Note: In case of expired nibbles Start of the highest interval is greater than the Last update of the previous interval because each nibble has a certain timeout for its delivery. In case a computation od some nibble was terminated before its completion, a new nibble form a new interval may be taken before completion of expired nibble. 



</body>
</html>