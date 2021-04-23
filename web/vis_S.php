<!DOCTYPE html>
<html>
<head>

<?php    
$servername = "localhost";
$username = "id16232074_main";
$password = "CoSemJenomDat17!";
$dbname = "id16232074_prime17";

$nbeg = "";
$nend = "";
$sequence = "Both";
$table = "S";
$split = '';


if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $nbeg = test_input($_POST["nbeg"]);
    $nend = test_input($_POST["nend"]);
    $sequence = test_input($_POST["sequence"]);
    $table = test_input($_POST["table"]);

}

function test_input($data) {
  $data = trim($data);
  $data = stripslashes($data);
  $data = htmlspecialchars($data);
  return $data;
}

function dynamic_sql_N($inp, $op){
    $inp = trim($inp);
    if (stristr($inp,',')) {
        return ' and' .' N in ('  .  $inp . ') ';   
    }  elseif ($inp<>'') {
        return ' and N ' . $op . $inp . ' ';
    }  else return '';  
}


try {
    $conn = new PDO("mysql:host=$servername;dbname=$dbname", $username, $password);
    $conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    

    if ($table<>'T' && $table<>'TT') {
        $table = 'S';
    }  
    $sql = 'select * from ' . ' v_' . $table . ' where 1=1 ';
 
    if ($sequence === 'Odd') $sql .= ' and Odd = 1 ';
    elseif ($sequence === 'Even') $sql .= ' and Odd = 0 ';
    
 
    $sql .= dynamic_sql_N($nbeg, '>=');
    $sql .= dynamic_sql_N($nend, '<=');
    
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
?>    
    
    <script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script>
    <script type="text/javascript">
        google.charts.load('current', {'packages':['corechart']});
        // google.charts.load('current', {'packages':['line']});
        google.charts.setOnLoadCallback(drawChart);

        function drawChart() {
            <?php          
            $col = 'dqs0';
            echo('var col="' . $col . '"');
            $split = '';
            
            if ($split =='') {$header = "['N',col]";}
            else {$header = "['N',col,'XX']";}
            ?>
            
            
            var data = google.visualization.arrayToDataTable([

            <?php          
                echo($header);
                foreach($rs as $r) {
                    echo ',[' . $r['N'] . ','; 
                    
                    if ($split=='') echo  $r['dqs0'];
                    else {
                        if ($r['N'] & 1) echo $r['dqs0'] . ', null';
                        else            echo  'null,' . $r['dqs0'];
                    }
                    
                    
                    echo  ']';
                }
            ?>
            ]);
            
            var options = {
                title: 'Deviation of Super slice per various intervals',
                //curveType: 'function',
                legend: { position: 'bottom' }
            };
            
            var chart = new google.visualization.LineChart(document.getElementById('curve_chart'));
            // var chart = new google.charts.Line(document.getElementById('curve_chart'));
            var chart = new google.visualization.ColumnChart(document.getElementById("curve_chart"));
            chart.draw(data, options);
        }
</script>
</head>
<body>
    
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
    <label for="nbeg">N start:</label><br>  <input type="text" id="nbeg" name="nbeg" value = <?php echo($nbeg)?>><br>
    <label for="nend">N end:</label><br>   <input type="text" id="nend" name="nend" value = <?php echo($nend)?>><br>

    <input type="radio" id="Even" name="sequence" value="Even" <?php echo($sequence=='Even' ? 'checked':'unchecked'); ?>><label for="Even">Even</label><br>
    <input type="radio" id="Odd" name="sequence" value="Odd" <?php echo($sequence=='Odd' ? 'checked':'unchecked'); ?>><label for="Odd">Odd</label><br>
    <input type="radio" id="Both" name="sequence" value="Both" <?php echo($sequence=='Both' ? 'checked':'unchecked'); ?>><label for="Both">Both</label><br> 
    <br>
    <input type="radio" id="T" name="table" value="T" <?php echo($table=='T' ? 'checked':'unchecked'); ?>><label for="T">T</label><br>
    <input type="radio" id="TT" name="table" value="TT" <?php echo($table=='TT' ? 'checked':'unchecked'); ?>><label for="TT">TT</label><br>
    <input type="radio" id="S" name="table" value="S" <?php echo($table=='S' ? 'checked':'unchecked'); ?>><label for="S">S</label><br> 
    

    <input type="submit" value="Submit">
</form>



<div id="curve_chart" style="width: 1800px; height: 800px"></div>

</br>

This is a report on the .</br></br>
<?php
echo $nend;
echo $sql;
echo '----seq ';
echo '--------------';
?>

Legend:</br>

</br> 

Note:



</body>
</html>