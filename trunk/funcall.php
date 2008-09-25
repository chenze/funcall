<?php 
function trim_post_cb($args) {
    var_dump($args);
}
fc_add_pre('include','trim_post_cb');
require_once('a.php');
?>
