<?php 
require_once 'utils.php';
require_once 'sql.php';
#require_once 'vis_function.php';
#require_once 'vis_form_function.php';

function dynamic_sql_N($col, $inp, $op){
    $inp = trim($inp);
    if (stristr($inp,',')) {
        return ' and ' . $col . ' in ('  .  $inp . ') ';   
    }  elseif ($inp<>'') {
        return ' and ' .$col .  $op . $inp . ' ';
    }  else return '';  
}

function vis_post_setvar($chart, ?string $variable=null, $variablename) {
    global ${$chart . $variablename}; 
    if (!isset(${$chart . $variablename})) {
        if ($variablename ==='table') ${$chart . $variablename} = "S";
        elseif ($variablename ==='column') ${$chart . $variablename} = "F";
        elseif ($variablename ==='sequence') ${$chart . $variablename} = "All";
        else ${$chart . $variablename} = '';
    }
    
    if (isset($variable)) ${$chart . $variablename} = $variable;
    
    if ($_SERVER["REQUEST_METHOD"] == "POST") {
        if (isset($_POST[$chart . $variablename])) ${$chart . $variablename} = test_input($_POST[$chart . $variablename]);
    }
    return ${$chart . $variablename};
}

function vis_head($chart, ?string $nbeg=null ,?string $nend=null, ?string $sequence=null, ?string $table=null, ?string $column=null, ?string $xbeg = null, ?string $xend = null ){
 
    // echo $column . ' jjjj';
    $nbeg = vis_post_setvar($chart, $nbeg, 'nbeg');
    $nend = vis_post_setvar($chart, $nend, 'nend');
    $xbeg = vis_post_setvar($chart, $xbeg, 'xbeg');
    $xend = vis_post_setvar($chart, $xend, 'xend');
    $table = vis_post_setvar($chart, $table, 'table');
    $column = vis_post_setvar($chart, $column, 'column');
    $sequence = vis_post_setvar($chart, $sequence, 'sequence');
    
    if ($table=='S') {
        $idx='';
        switch ($column){
            case 'T': $column = 'T_Ns'; break;
            case 'pi': $column = 'pi_N'; break;
            case 'pi_s': $column = 'pi_Ns0'; break;
            case 'q': $column = 'qs0'; break;
            case 'Z': $column = 'Z_Ns0'; break; // 
            case 'F': $column = 'F_Ns0'; break;
            case 'V': $column = 'V_Ns0'; break;
            case 'L': $column = 'L_Ns'; break;
            case 'P': $column = 'P_Ns0'; break;
        }
    } else if ($table == 'TT') {
        $idx='mm';
        switch ($column){
            case 'pi': $column = 'pi_N'; break;
            case 'T': $column = 'TT_Nj'; break;
            case 'pi_s': $column = 'ppi_Nj'; break;
            case 'q': $column = 'qq_Nj'; break;
            case 'Z': $column = 'ZZ_Nj'; break;   
            case 'F': $column = 'FF_Nj'; break;
            case 'V': $column = 'VV_Nj'; break;
            case 'L': $column = 'LL_Nj'; break;
            case 'P': $column = 'PP_Nj'; break;
        }
    } else if ($table == 'T') {
        $idx='m';
        switch ($column){
            		
            case 'pi': $column = 'pi_N'; break;
            case 'T': $column = 'T_Nj'; break;
            case 'pi_s': $column = 'pi_Nj'; break;
            case 'q': $column = 'q_Nj'; break;
            case 'Z': $column = 'Z_Nj'; break;  
            case 'F': $column = 'F_Nj'; break;
            case 'V': $column = 'V_Nj'; break;
            case 'L': $column = 'L_Nj'; break;
            case 'P': $column = 'P_Nj'; break;
        }
    }
    
    $sql = 'select N, ';
    if ($idx!=='') $sql .= $idx . ', ';
    $sql .= $column . ' from ' . ' v_' . $table . ' where 1=1 ';
    
    if ($sequence === 'Odd') $sql .= ' and Odd in (1,3) ';
    elseif ($sequence === 'Even') $sql .= ' and Odd in (0,2) ';
    elseif ($sequence === 'EvenEven') $sql .= ' and Odd in (0) ';
    elseif ($sequence === 'EvenOdd') $sql .= ' and Odd in (2) ';
    elseif ($sequence === 'OddEven') $sql .= ' and Odd in (1) ';
    elseif ($sequence === 'OddOdd') $sql .= ' and Odd in (3) ';
    
    
    $sql .= dynamic_sql_N('N', $nbeg, '>=');
    $sql .= dynamic_sql_N('N', $nend, '<=');
    
    $axisX = $idx=='' ? 'N' : $idx;
    $sql .= dynamic_sql_N($axisX, $xbeg, '>=');
    $sql .= dynamic_sql_N($axisX, $xend, '<=');
    
    
    echo  '<link rel="stylesheet" href="visualise.css"> ';
        
    // echo $sql;
    $rs = ExecSQL($sql);
    // print_r($rs);
    
    $split = 'yes';
    if ($split !== '' and $sequence ==='All')   {$header = "['N','odd','even']";}
    else  {$header = "['N', '" . $sequence . "']";}
    
    
    

    echo '<script type="text/javascript" src="https://www.gstatic.com/charts/loader.js"></script> ';
    #echo nl2br("\n" . PHP_EOL .PHP_EOL );
    echo '<script type="text/javascript"> ';
    echo "google.charts.load('current', {'packages':['corechart']}); ";
    echo 'google.charts.setOnLoadCallback(draw' . $chart . '); ';

    echo 'function draw' . $chart . '() { ';
    
    echo 'var data = new google.visualization.DataTable(); ';
    
    
    if ($table == 'T' or $table == 'TT' ) {
        $OldN = 0; // 0 is not a valid interval
        $row = [];
        $tbl = [];
        // range of X axis as a global min and max from all series
        $axisX = [];
        $idx_min = null;
        $idx_max = null;
        
        foreach($rs as $r) {
            $NewN=$r['N']; 
            $axisX[$r[$idx]] = $r[$idx];
            
            // print_r($r);
            if ($NewN<>$OldN) {
                if (count($row)>0) {
                    // $tbl["'" . $OldN . "'"] = $row;
                    $tbl[ $OldN ] = $row;
                    // print_r($r);
                }
                $row=[]; 
            }
            $OldN = $NewN;
            $row[$r[$idx]] = $r[$column];
        }
        // add last row
        if (count($row)>0) {
            $tbl[$OldN ] = $row;
            // print_r($r);
        }

        
        // print_r($row);
        // echo $row[0];
        sort($axisX);
        // print_r($axisX);
        // print_r($rs);
        // print_r($tbl);
    
        $pivot = [];
        foreach($tbl as $N => $r){
            foreach($axisX as $X){
                $pivot[$X][$N] = isset($r[$X]) ?$r[$X] : null;   
            }
        }
        // print_r($pivot);
        echo "data.addColumn('number', '" . $idx . "'); ";
        foreach (array_values($pivot)[0]   as $X => $r) echo "data.addColumn('number', '" . $X . "'); ";
        
        $First = '';
        echo 'data.addRows([ ';
        foreach($pivot as $X => $r){
            echo $First . '[';
            $First = ', ';
            echo $X;
            foreach ($r as $val){
                echo ', ';
                echo isset($val) ? $val : 'null';   
            }
            echo  ']';
            
        }
        
        echo ']); ';
    // print_r($pivot);
    
    } else {    
        echo "data.addColumn('number', 'N'); ";
    
        if ($split !== '' and $sequence ==='All')   {
            echo "data.addColumn('number', 'Odd'); ";
            echo "data.addColumn('number', 'Even'); ";
            
        } else  {
            echo "data.addColumn('number', '" . $sequence . "'); ";
        }
    
        echo 'data.addRows([ ';
        
        $First = "";
        foreach($rs as $r) {
            echo $First;
            $First  = ", ";
            echo '[' . $r['N'] . ','; 
                
            if ($split !== '' and $sequence ==='All') 
            {
                if ($r['N'] & 1) echo $r[$column] . ', null';
                else             echo  'null,' . $r[$column];
            } else {
                echo  $r[$column];
            }
            echo  ']';
        }
        echo ']); ';
    }

    echo 'var options = { ';
    echo "title: 'slice: " . $table . " data: " . $column . "   sequence: " . $sequence . "' ";
    #echo "curveType: 'function',";
    echo ", legend: { position: 'bottom' } ";
    echo ", crosshair: { color: '#000', trigger: 'selection'  } ";
    echo ", interpolateNulls: true";
    echo ", explorer: { axis: 'vertical', keepInBounds: true, maxZoomOut: 1, maxZoomIn: .00001, zoomDelta: 1.05 } ";
    // echo ", explorer: { keepInBounds: true, maxZoomOut: 1, maxZoomIn: .00001, zoomDelta: 1.05 } ";
    

    echo '}; ';
    
    
            
    echo 'var chart = new google.visualization.LineChart(document.getElementById("' . $chart .'")); ';
    // echo 'var chart = new google.visualization.ColumnChart(document.getElementById("' . $chart .'")); ';
            
    echo 'chart.draw(data, options); ';
    echo '} ';
    echo '</script> ';
}



function vis_form($chart, ?string $nbeg = null, ?string $nend = null, ?string $sequence = null, ?string $table = null, ?string $column = null, ?string $xbeg = null, ?string $xend = null ){
    $nbeg = vis_post_setvar($chart, $nbeg, 'nbeg');
    $nend = vis_post_setvar($chart, $nend, 'nend');
    $xbeg = vis_post_setvar($chart, $xbeg, 'xbeg');
    $xend = vis_post_setvar($chart, $nend, 'xend');
    $sequence = vis_post_setvar($chart, $sequence, 'sequence');
    $table = vis_post_setvar($chart, $table, 'table');
    $column = vis_post_setvar($chart, $column, 'column');
    
    
    // echo '<fieldset><legend>N Range</legend>';
        // echo '<div title="eg. 10 or 10,14,18"> ';
        echo '<label for="' .$chart . 'nbeg">N start:'; 
        echo '<input type="text" id="' . $chart . 'nbeg" name="' . $chart . 'nbeg" ';
        echo $nbeg=='' ? '' : 'value = ' . $nbeg ;
        echo '> </label>';
        // echo '</div> ' ;
        // echo '<br> ';
        
        echo '<label for="' .$chart . 'nend">N end:'; 
        echo '<input type="text" id="' . $chart . 'nend" name="' . $chart . 'nend" ';
        echo $nend=='' ? '' : 'value = ' . $nend ;
        echo '> </label> ' ;
        // echo '<br> ';
    // echo '</fieldset>';


    echo '<label for="' .$chart . 'xbeg">X beg:'; 
    echo '<input type="text" id="' . $chart . 'xbeg" name="' . $chart . 'xbeg" ';
    echo $xbeg=='' ? '' : 'value = ' . $xbeg ;
    echo '> </label> ' ;
    
    echo '<label for="' .$chart . 'xend">X end:'; 
    echo '<input type="text" id="' . $chart . 'xend" name="' . $chart . 'xend" ';
    echo $xend=='' ? '' : 'value = ' . $xend ;
    echo '> </label> ' ;
    echo '<br>';

    
    // echo '<fieldset><legend>Sequence</legend>';
    //     echo '<input type="radio" id="' . $chart . 'Even" name="' . $chart . 'sequence" value="Even" ' ;
    //     echo $sequence == 'Even' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'Even">Even</label><br> ';
        
    //     echo '<input type="radio" id="' . $chart . 'Odd" name="' . $chart . 'sequence" value="Odd" ' ;
    //     echo $sequence == 'Odd' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'Odd">Odd</label><br> ';
        
    //     echo '<input type="radio" id="' . $chart . 'Both" name="' . $chart . 'sequence" value="Both" ' ;
    //     echo $sequence == 'Both' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'Both">Both</label><br> ';
    // echo '</fieldset>';

    // echo '</br>';
    
    // echo '<fieldset><legend>Table</legend>';
    //     echo '<input type="radio" id="' . $chart . 'T" name="' . $chart . 'table" value="T" ' ;
    //     echo $table == 'T' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'T">T</label><br> ';
    
    //     echo '<input type="radio" id="' . $chart . 'TT" name="' . $chart . 'table" value="TT" ' ;
    //     echo $table == 'TT' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'TT">TT</label><br> ';
    
    //     echo '<input type="radio" id="' . $chart . 'S" name="' . $chart . 'table" value="S" ' ;
    //     echo $table == 'S' ? 'checked' : 'unchecked';
    //     echo '><label for="' . $chart . 'S">S</label><br> ';
    // echo '</fieldset>';
    
    echo '<label for="' . $chart . 'table">Slice:';
    echo '<select name="' . $chart . 'table" id="table"> ';
        echo_option('T', $table);
        echo_option('TT', $table);
        echo_option('S', $table);
    echo '</select> </label>  ';
    
    
    echo '<label for="' . $chart . 'column">Data:';
    echo '<select name="' . $chart . 'column" id="column"> ';
        echo_option('F', $column);
        echo_option('q', $column);
        echo_option('V', $column);
        echo_option('L', $column);
        echo_option('P', $column);
        echo_option('pi', $column);
        echo_option('pi_s', $column);
        echo_option('Z', $column);
    echo '</select> </label> ';
    
    echo '<label for="' . $chart . 'sequence">Sequence:';
    echo '<select name="' . $chart . 'sequence" id="sequence"> ';
        echo_option('All', $sequence);
        echo_option('Even', $sequence);
        echo_option('Odd', $sequence);
        echo_option('EvenOdd', $sequence);
        echo_option('EvenEven', $sequence);
        echo_option('OddOdd', $sequence);
        echo_option('OddEven', $sequence);
    echo '</select> </label> ';
    
    
    echo '<input type="submit" value="Submit"> ';

}

function echo_option($v, $c){
    echo '<option value="' . $v . '"';
    if ($c == $v) echo  ' selected="selected"'; 
    echo '>' . $v . '</option> ';
}


?>    
    
