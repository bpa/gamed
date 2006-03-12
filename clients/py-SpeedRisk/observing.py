import traceback

class Observable:
  def add_observer(self, object, fields=None):
    if not self.__dict__.has_key('_Observable__observers'):
      self.__observers = {}
    fhash = {}
    if fields is not None:
      for f in fields:
        fhash[f] = 0
    self.__observers[object] = fhash

  def add_watch(self, fields=None):
    self.add_observer(self, fields)

  def handle_observation(self, obj, field, old, new):
    tb = traceback.extract_stack(None,3)[0]
    watch = "Watch for %s" % self.__class__.__name__
    print watch.center(len(watch)+2).center(78,'*')
    print "%s line %s:" % (tb[0], tb[1])
    print tb[3]
    print "%s => %s" % (old, new)
    
  def __setattr__(self, field, val):
    if self.__dict__.has_key(field):
      old = self.__dict__[field]
    else:
      old = None
    self.__dict__[field] = val
    if self.__dict__.has_key('_Observable__observers'):
      obs = self.__observers
      for o in obs.keys():
        if len(obs[o]) == 0 or obs[o].has_key(field):
          o.handle_observation(self, field, old, val)
