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

vis_head('chart1_', 25, null, 'All', 'S', 'F', -8);


?>

</head>

<body>
<form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
<?php
vis_form('chart1_');
?>
</br>
<div id="chart1_" style="width: 1200px; height: 600px"></div>



</form>
</body>
</html>