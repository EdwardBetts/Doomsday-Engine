<?php
    require_once('class.session.php');
    require_once('template.inc.php');
    
    // Check the latest build.
    $user_platform = detect_user_platform();
    $ckey = cache_key('home', ['topbar_dl', $user_platform]);
    if (!cache_try_load($ckey)) {
        $dl_link = platform_download_link();
        // Find out the latest stable build.
        $db = Session::get()->database();
        $result = db_query($db, "SELECT version FROM ".DB_TABLE_BUILDS
            ." WHERE type=".BT_STABLE." ORDER BY timestamp DESC LIMIT 1");
        if ($row = $result->fetch_assoc()) {
            $button_label = omit_zeroes($row['version']);
        }
        else {
            $button_label = 'Download';            
        }    
        cache_echo("<a href='$dl_link' class='quick-download-link' "
            ."title='Download latest stable release'>$button_label <span "
            ."class='downarrow'>&#x21E3;</span></a>");
        cache_store($ckey);
    }            
    $download_link = cache_get();
?>
<div id="dengine-topbar">
    <ul class="site-navigation">
        <li><a href="<?php echo(SITE_ROOT); ?>" class="site-link">Doomsday<span class="aux-word"> Engine</span></a></li>
        <li class="quick-download"><?php echo($download_link); ?></li>
        <li class="supplementary"><a href="<?php echo(SITE_ROOT); ?>/builds">Builds</a></li>
        <li class="supplementary"><a href="<?php echo(SITE_ROOT."/addons"); ?>">Add-ons</a></li>
        <li><a href="https://manual.dengine.net">Manual</a></li>
        <li>&middot;</li>
        <li><a href="<?php echo(SITE_ROOT); ?>/talk">Forums</a></li>
        <li class="supplementary"><a href="<?php echo(SITE_ROOT); ?>/support">Support</a></li>
        <li><a href="https://tracker.dengine.net/projects/deng"><span class="aux-word">Bug </span>Tracker</a></li>
        <li><a href="<?php echo(SITE_ROOT).'/recent_posts'; ?>" class="blog-link">Blog</a></li>
    </ul>
</div>
