# Start up script to initalize database
size=1

#if test -f "deployment.xml"; then rm -f deployment.xml; fi

# Create default deployment file
#printf "<?xml version=\"1.0\"?>
#    <deployment>
#        <cluster hostcount=\"1\" sitesperhost=\"%s\" kfactor=\"0\" />
#        <httpd enabled=\"true\">
#            <jsonapi enabled=\"true\" />
#        </httpd>
#        </deployment>\n" "$size" >> deployment.xml

# Voltdb Default Ports http=8080 admin=21211 client=21212 internal=3021 jmx=9090 zookeeper=7181
~/voltdb/bin/voltdb create --deployment=deployment.xml

