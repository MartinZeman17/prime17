<!DOCTYPE html>
<html>
<body>

<?php
require_once 'utils.php';
require_once 'sql.php';

class TableRows extends RecursiveIteratorIterator {
    function __construct($it) {
        parent::__construct($it, self::LEAVES_ONLY);
    }

    function current() {
        return "<td style='width: 150px; border: 1px solid black;'>" . parent::current() . "</td>";
    }
    
    function key() {
        return "<td style='width: 150px; border: 1px solid black;'>" . parent::key() . "</td>";
    }

    function beginChildren() {
        echo "<tr>";
    }

    function endChildren() {
        echo "</tr>" . "\n";
    }
}

echo "<table style='border: solid 1px black;'>";
echo " <table><tr>";

echo  ' <th>N</th> ' ;
echo  ' <th>Odd</th> ' ;
echo  ' <th>T_Ns</th> ' ;
echo  ' <th>pi_N</th> ' ;
echo  ' <th>pi_Ns0</th> ' ;
echo  ' <th>pi_Ns1</th> ' ;
echo  ' <th>qs0</th> ' ;
echo  ' <th>qs1</th> ' ;
echo  ' <th>Z_N</th> ' ;
echo  ' <th>V_Ns0</th> ' ;
echo  ' <th>V_Ns1</th> ' ;
echo  ' <th>F_Ns0</th> ' ;
echo  ' <th>F_Ns1</th> ' ;
echo  ' <th>L_Ns</th> ' ;
echo  ' <th>P_Ns0</th> ' ;
echo  ' <th>P_Ns1</th> ' ;


echo ' </tr>';

$rs = ExecSQL("select * from v_S");

foreach(new TableRows(new RecursiveArrayIterator($rs) )  as $k=>$v )     {
    echo $v ;
}
    
echo "</table>";

include("work_out_S_info.php"); ?>

</body>
</html>