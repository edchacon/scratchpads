Drupal.hostmaster_tweak = new Object;

Drupal.hostmaster_tweak.do_search = function(copy){
  if(copy){
    var domain = $('#edit-title-copy').val();    
  }else{
    var domain = $('#edit-title').val();
  }
  if(domain.length>10){
    if(domain.substring(domain.length-10) == 'taxon.name'){
      $('#edit-profile-696').attr('checked', 'checked');
      $('#edit-platform-767').attr('checked', 'checked');
      $('#edit-db-server-721').attr('checked', 'checked');
    }    
  }
  return false;
}

Drupal.behaviors.hostmaster_tweak = function(context){
  $('#edit-profile-691', context).attr('checked', 'checked');
  $('#edit-title', context).keyup(function(){
    Drupal.hostmaster_tweak.do_search(false);
    return false;
  });
  $('#edit-title-copy', context).keyup(function(){
    Drupal.hostmaster_tweak.do_search(true);
    return false;
  });
};