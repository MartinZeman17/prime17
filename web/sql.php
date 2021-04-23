<?php 

function ExecSQL($sql){
    $servername = "localhost";
    $username = "id16232074_main";
    $password = "CoSemJenomDat17!";
    $dbname = "id16232074_prime17";

    $rs = null;
    try {
        $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
        $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
        
        $stmt = $conn->prepare($sql);
        $stmt->execute();
    
        // set the resulting array to associative
        $result = $stmt->setFetchMode(PDO::FETCH_ASSOC);
        $rs = $stmt->fetchAll();
    }
    catch(PDOException $e) {
        echo "Error: " . $e->getMessage();
    }
    $conn = null;
    return $rs;
}
?>    
