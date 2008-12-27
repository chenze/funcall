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
function pre_monitor($args) {
    echo 'pre:',$args[0],"\n";
}
function post_monitor($args,$result,$time) {
    echo 'post:',$result,"\n";
}
function getCommand() {
    return 'echo "xyz";';
}
fc_add_pre('eval','pre_monitor');
fc_add_post('eval','post_monitor');
$command='echo "abc"';
eval($command.';');
$command='echo "def";return "xxx";';
eval($command);
eval(getCommand());
eval('echo "tgz";return "ok";');
?>
--EXPECT--
pre:echo "abc";
abcpost:
pre:echo "def";return "xxx";
defpost:xxx
pre:echo "xyz";
xyzpost:
pre:echo "tgz";return "ok";
tgzpost:ok
