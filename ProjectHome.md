## functions ##
### boolean fc\_add\_pre(string function,string callback) ###
Add a callback which will be called before the function call.The callback accept one parameter,which is array contain the args passed to function.See examples below for detail.

### boolean fc\_add\_post(string function,string callback) ###
Add a callback which will be called after the function call.The callback accept three parameters.First one is array contain the args passed to function, the second param is the result of that function and the third one is the processing time in ms of that function.See examples below for detail.


### array fc\_list() ###
> Return all callbacks like:
```
    array(
        0=>array(
            'mysql_query'=>array(
                'pre_callback1','pre_callback2'
            )
        ),
        1=>array(
            'mysql_query'=>array(
                'post_callback1','post_callback2'
            )
        )
    );
```


[Examples](http://code.google.com/p/funcall/wiki/Example)