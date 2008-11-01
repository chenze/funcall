<?php 
class a{
    function bb() {
        echo "bb\n";
    }
}
function pre_cb($args) {
    var_dump($args);
}
function post_cb($args,$result,$t) {
    var_dump($args);
}
fc_add_pre('include','pre_cb');
fc_add_post('include','post_cb');
fc_add_pre('a::bb','pre_cb');
fc_add_post('a::bb','post_cb');
print_r(fc_list());
require_once('a.php');
a::bb();
?>
