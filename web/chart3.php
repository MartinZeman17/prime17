<!DOCTYPE html>
<html>
<head>
<?php

require_once 'visualise.php';
// $chart, ?string $nbeg=null ,?string $nend=null, ?string $sequence=null, ?string $table=null, ?string $column=null, ?string $xbeg = null, ?string $xend = null )

// $chart1_nbeg=10;
// $chart1_nend='';
// $chart1_sequence ='All';
// $chart1_table = 'S';

vis_head('chart1_', 20, null, 'All', 'TT', 'F', -8);
vis_head('chart2_', 20, null, 'Odd', 'TT', 'F', -8);
vis_head('chart3_', 20, null, 'Even', 'TT', 'F', -8);

?>

</head>

<body>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
<?php
vis_form('chart1_');
?>
</br>
<div id="chart1_" style="width: 1200px; height: 600px"></div>

<?php
vis_form('chart2_');
?>
</br>
<div id="chart2_" style="width: 1200px; height: 600px"></div>

<?php
vis_form('chart3_');
?>
</br>
<div id="chart3_" style="width: 1200px; height: 600px"></div>


</form>
</body>
</html>