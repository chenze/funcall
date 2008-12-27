--TEST--
Checking include
--SKIPIF--
<?php if (!extension_loaded("funcall")) print "skip"; ?>
--FILE--
<?php
touch('/tmp/funcall_include.php');
touch('/tmp/funcall_include2.php');
touch('/tmp/funcall_include3.php');
touch('/tmp/funcall_include4.php');
function include_monitor($path) {
    echo 'pre:',$path[0],"\n";
}
function include_monitor_post($path,$result,$time) {
    echo 'post:',$path[0],"\n";
}

$include_prefix='funcall_include';
$include_file='funcall_include.php';
$include_full_path='/tmp/funcall_include.php';
$include4_full_path='/tmp/funcall_include4.php';
function includePath() {
    return '/tmp/funcall_include.php';
}

fc_add_pre('include','include_monitor');
fc_add_pre('require','include_monitor');
fc_add_pre('include_once','include_monitor');
fc_add_pre('require_once','include_monitor');
fc_add_post('include','include_monitor_post');
fc_add_post('require','include_monitor_post');
fc_add_post('include_once','include_monitor_post');
fc_add_post('require_once','include_monitor_post');
include includePath();//is_var
include '/tmp/funcall_include.php';//is_const
include_once '/tmp/funcall_include2.php';//is_const
include_once '/tmp/funcall_include2.php';//is_const
require_once '/tmp/'.$include_prefix.'3.php';//is_tmpvar
require_once '/tmp/funcall_include3.php';//is_const
include '/tmp/'.$include_file;//is_tmpvar
include $include_full_path;//is_cv
require_once $include4_full_path;//is_cv
require '/tmp/funcall_include.php';//is_const
require '/tmp/'.$include_file;//is_tmpvar
require $include_full_path;//is_cv
require includePath();//is_var
?>
--EXPECT--
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include2.php
post:/tmp/funcall_include2.php
pre:/tmp/funcall_include3.php
post:/tmp/funcall_include3.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include4.php
post:/tmp/funcall_include4.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
pre:/tmp/funcall_include.php
post:/tmp/funcall_include.php
