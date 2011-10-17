
#ifndef _GK_PROFILER_CLOCK
#define _GK_PROFILER_CLOCK

#include <string>
#include <map>
#include <vector>
#include <cstdio>
#include <cassert>
#include <limits.h>

#ifdef APPLE_OSX
    #include <sys/time.h>
    
#elif defined WIN32
    #include <windows.h>
    
#else
    #include <sys/time.h>
#endif

#include <cstdlib>

namespace gk {
    
#ifndef WIN32
// posix version
//! horloge systeme, duree mesuree en micro secondes.
class ProfilerClock
{
    // non copyable
    ProfilerClock( const ProfilerClock& );
    ProfilerClock& operator=( const ProfilerClock& );
    
protected:
    struct timeval m_start;

    //! constructeur prive, singleton, cf manager().
    ProfilerClock( )
    {
        gettimeofday(&m_start, NULL);
    }
    
    //! destructeur.
    ~ProfilerClock() {}

    int delay( const struct timeval& x, const struct timeval& base ) const
    {
        struct timeval y= base;
        
        /* Perform the carry for the later subtraction by updating y. */
        if(x.tv_usec < y.tv_usec)
        {
            int nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
            y.tv_usec -= 1000000 * nsec;
            y.tv_sec += nsec;
        }
        
        if (x.tv_usec - y.tv_usec > 1000000)
        {
            int nsec = (x.tv_usec - y.tv_usec) / 1000000;
            y.tv_usec += 1000000 * nsec;
            y.tv_sec -= nsec;
        }
        
        int sec = x.tv_sec - y.tv_sec;
        int usec = x.tv_usec - y.tv_usec;
        
        return sec * 1000000 + usec;
    }

    int get_ticks( )
    {
        struct timeval ticks;
        gettimeofday(&ticks, NULL);
        
        //~ printf("ticks %llus %dus - %llus %dus= %dus\n",
            //~ (long long unsigned int) ticks.tv_sec, (int) ticks.tv_usec,
            //~ (long long unsigned int) m_start.tv_sec, (int) m_start.tv_usec,
            //~ delay(ticks, m_start));
        
        return delay(ticks, m_start);
    }

    int get_delay( const unsigned int start )
    {
        int stop= get_ticks();
        return stop - start;
    }
    
public:
    //! type des mesures renvoyee par l'horloge systeme.
    typedef int Ticks;

    //! renvoie l'horloge systeme.
    static
    Ticks getTicks( )
    {
        return manager().get_ticks();
    }

    //! renvoie un delai en micro secondes entre le moment actuel et 'base'.
    static 
    int getDelay( const Ticks base )
    {
        return manager().get_delay(base);
    }
    
    //! calcule un delai en micro-secondes entre 'start' et 'stop'.
    static 
    int delay( const Ticks stop, const Ticks start )
    {
        return stop - start;
    }
    
    //! interface du singleton.
    static
    ProfilerClock& manager( )  // singleton
    {
        static ProfilerClock manager;
        return manager;
    }    
};

#else
// win32 version
class ProfilerClock
{
    // non copyable
    ProfilerClock( const ProfilerClock& );
    ProfilerClock& operator=( const ProfilerClock& );
    
    struct clock_val
    {
        int sec;
        int usec;
    };

    int delay( const clock_val& x, const clock_val& base ) const
    {
        clock_val y= base;
        
        /* Perform the carry for the later subtraction by updating y. */
        if(x.usec < y.usec)
        {
            int nsec = (y.usec - x.usec) / 1000000 + 1;
            y.usec -= 1000000 * nsec;
            y.sec += nsec;
        }
        
        if (x.usec - y.usec > 1000000)
        {
            int nsec = (x.usec - y.usec) / 1000000;
            y.usec += 1000000 * nsec;
            y.sec -= nsec;
        }
        
        int sec = x.sec - y.sec;
        int usec = x.usec - y.usec;
        
        return sec * 1000000 + usec;
    }
    
protected:
    clock_val m_start;
    LARGE_INTEGER m_last_frequency;

    ProfilerClock( ) 
    {
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        m_last_frequency= frequency;
        
        m_start.sec= ticks.QuadPart / frequency.QuadPart;
        //~ m_start.usec= (ticks.QuadPart % frequency.QuadPart) * 1000000 / frequency.QuadPart;
        m_start.usec= (ticks.QuadPart % frequency.QuadPart) / (frequency.QuadPart / 1000000);
    }
    
    ~ProfilerClock() {}

    int get_ticks( )
    {
        LARGE_INTEGER ticks;
        QueryPerformanceCounter(&ticks);
        
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        if(frequency.QuadPart != m_last_frequency.QuadPart)
        {
        #if VERBOSE
            printf("ProfilerClock( ): frequency scaling...\n");
        #endif
            m_last_frequency= frequency;
        }
        
        clock_val stop;
        stop.sec= ticks.QuadPart / frequency.QuadPart;
        //~ stop.usec= (ticks.QuadPart % frequency.QuadPart) * 1000000 / frequency.QuadPart;
        stop.usec= (ticks.QuadPart % frequency.QuadPart) / (frequency.QuadPart / 1000000);
        return delay(stop, m_start);
    }
    
    int get_delay( const int base )
    {
        int stop= get_ticks();
        return stop - base;
    }
    
public:
    //! type des mesures renvoyee par l'horloge systeme.
    typedef int Ticks;

    static
    Ticks getTicks( )
    {
        return manager().get_ticks();
    }

    static 
    int getDelay( const Ticks base )
    {
        return manager().get_delay(base);
    }
    
    static 
    int delay( const Ticks stop, const Ticks start )
    {
        return stop - start;
    }
    
    static
    ProfilerClock& manager( )  // singleton
    {
        static ProfilerClock manager;
        return manager;
    }    
};

#endif

//! classe de base des donnees utilisateur associees a un historique de valeurs / cf StatsCounter.
//! permet d'accrocher des informations supplementaires a un StatCounter.
//! les "donnees", une classe derivee de IStatsUserData, sont allouees par l'application et deviennent propriete du StatsCounter.
class IStatsUserData
{
protected:
    std::string m_name;
    
public:
    IStatsUserData( const std::string& name )
        :
        m_name(name)
    {}

    const std::string& name( ) const
    {
        return m_name;
    }
    
    virtual ~IStatsUserData( ) {}
};

//! stocke un historique de valeurs.
class StatsCounter
{
    std::vector<IStatsUserData *> m_data;
    
    long long int *m_stats;
    int m_head;
    int m_tail;
    int m_size;
    int m_n;
    
    int m_min;
    int m_max;
    long long int m_sum;

    // non copyable
    StatsCounter( const StatsCounter& );
    StatsCounter& operator=( const StatsCounter& );
    
public:
    //! constructeur, indique la taille de l'historique, le nombre de mesures conservees.
    StatsCounter( const int n= 100 )
        :
        m_head(0),
        m_tail(0),
        m_size(n),
        m_n(0),
        m_min(INT_MAX),
        m_max(INT_MIN),
        m_sum(0)
    {
        m_stats= new long long int[n];
    }
    
    //! destructeur. detruit egalement les donnees utilisateur, si necessaire, cf attachUserData().
    ~StatsCounter( )
    {
        delete [] m_stats;
        
        const int n= m_data.size();
        for(int i= 0; i < n; i++)
            delete m_data[i];
    }
    
    //! attache une donnee utilisateur a l'historique. renvoie son identifiant pour un acces direct, cf userData().
    int attachUserData( IStatsUserData *data )
    {
        if(data == NULL || findUserData(data->name()) != NULL)
            return -1;
        
        m_data.push_back(data);
        return 0;
    }
    
    //! recherche une donnee utilisateur, renvoie l'identifiant de la donnee -1 ou en cas d'echec.
    IStatsUserData *findUserData( const std::string& name )
    {
        return findUserData(name.c_str());
    }
    
    //! recherche une donnee utilisateur, renvoie l'identifiant de la donnee -1 ou en cas d'echec.
    IStatsUserData *findUserData( const char *name )
    {
        const int n= (int) m_data.size();
        for(int i= 0; i < n; i++)
            if(m_data[i]->name() == name)
                return m_data[i];
            
        return NULL;
    }
    
    //! renvoie la donnee utilisateur associee a l'historique, ou null en cas d'echec.
    IStatsUserData *userData( const int id ) 
    {
        if(id < 0 || id >= (int) m_data.size())
            return NULL;
        return m_data[id];
    }
    
    //! ajoute une mesure a l'historique.
    void push( const int value )
    {
        if(value < m_min)
            m_min= value;
        if(value > m_max)
            m_max= value;
        
        m_sum= m_sum + (long long int) value;
        if(m_n >= m_size)
        {
            // la file est pleine, retirer un element de la tete
            m_head= (m_head + 1) % m_size;
            m_n--;
            assert(m_n >= 0);
        }
        
        // ajouter le nouvel element en queue de file
        m_stats[m_tail]= m_sum;
        m_tail= (m_tail + 1) % m_size;
        m_n++;
        assert(m_n <= m_size);
    }
    
    //! renvoie les stats sur les mesures inserees.
    //! renvoie la mesure min, max et la moyenne glissante sur l'historique (les n dernieres mesures).
    //! les parametres peuvent etre NULL pour indiquer que la valeur n'est pas voulue.
    void getStats( int *min, float *average, int *max ) const
    {
        if(min != NULL)
            *min= m_min;
        if(max != NULL)
            *max= m_max;
        
        if(average != NULL)
            // moyenne glissante sur les n dernieres mesures
            *average= (m_n != 0) ? (float) (m_stats[(m_tail -1 + m_size) % m_size] - m_stats[m_head]) / (float) m_n : 0.f;
    }
    
    //! renvoie la moyenne glissante calculee sur les n dernieres mesures.
    float average( const unsigned int n= 30 ) const
    {
        if(n >= (unsigned int) m_n)
            return 0.f;
        
        const int i= (m_tail -1 + m_size) % m_size;      // indice 
        const int i1= (i - n + m_size) % m_size; // n valeurs avant
        return (m_stats[i] - m_stats[i1]) / (float) n;
    }
    
    //! renvoie la derniere mesure.
    int last( ) const
    {
        const int i= (m_tail -1 + m_size) % m_size;      // indice 
        const int i1= (i -1 + m_size) % m_size; // valeur precedente
        const long long int value= m_stats[i] - m_stats[i1];
        return (int) value;
    }
    
    //! renvoie le nombre de mesures dans l'historique.
    int statCount( ) const
    {
        return m_n -1;
    }
    
    //! renvoie la ieme mesure de l'historique. 0 pour la plus ancienne,  statCount() -1 pour la plus recente.
    int stat( const int id ) const
    {
        if(id < 0 || id >= m_n -1)
            return 0;
        
        const int i= (m_head + id +1) % m_size;      // indice 
        const int i1= (i -1 + m_size) % m_size; // valeur precedente
        return (int) (m_stats[i] - m_stats[i1]);
    }
    
    //! ecrit l'historique dans un fichier texte, une mesure par ligne, utilisable avec gnuplot, par exemple.
    int write( const std::string& name ) const
    {
        FILE *out= fopen(std::string(name+ ".txt").c_str(), "wt");
        if(out == NULL)
        {
            printf("error writing counter data '%s.txt'.\n", name.c_str());
            return -1;
        }
        
        int min, max;
        float av;
        getStats(&min, &av, &max);
        
        printf("counter '%s': min %d < %f < max %d,  ", name.c_str(), min, av, max);
        printf("writing counter history to '%s.txt'.\n", name.c_str());
        
        fprintf(out, "# min %d < %f < max %d\n", min, av, max);
        for(int id= 1; id < m_n; id++)
        {
            const int i= (m_head + id ) % m_size;      // indice 
            const int i1= (i -1 + m_size) % m_size; // valeur precedente
            const long long int value= m_stats[i] - m_stats[i1];
            
            fprintf(out, "%d\n", (int) value);
        }
        
        fclose(out);
        return 0;
    }
};


//! manager de compteurs : conserve l'ensemble des compteurs crees par 
//! l'application et ecrit leur historique dans des fichiers textes portant le nom es compteurs. 
class StatsCounterIO
{
    // non copyable
    StatsCounterIO( const StatsCounterIO& );
    StatsCounterIO& operator=( const StatsCounterIO& );

    std::map<std::string, StatsCounter *> m_counters_map;
    std::vector<StatsCounter *> m_counters;
    
    StatsCounterIO( ) {}
    
    ~StatsCounterIO( )
    {
        for(std::map<std::string, StatsCounter *>::iterator 
            i= m_counters_map.begin(); i != m_counters_map.end(); ++i)
        {
            // ecrit les stats dans un fichier texte ... gnuplot ?
            assert(i->second != NULL);
            i->second->write(i->first);
            delete i->second;
        }
    }
    
public:
    //! retrouve un compteur d'apres son nom.
    StatsCounter *find( const std::string& name )
    {
        std::map<std::string, StatsCounter *>::iterator found= m_counters_map.find(name);
        
        if(found == m_counters_map.end())
            return NULL;
        else
            return found->second;        
    }
    
    //! cree un nouveau compteur nomme conservant 'n' mesures.
    StatsCounter *create( const std::string& name, const int n )
    {
        StatsCounter *counter= new StatsCounter(n);
        m_counters_map.insert( std::make_pair( name, counter) );
        m_counters.push_back(counter);
        
        return counter;
    }

    //! renvoie le nombre de compteurs.
    int counterCount( ) const
    {
        return (int) m_counters.size();
    }
    
    //! renvoie le ieme compteur.
    StatsCounter *counter( const int id )
    {
        return m_counters[id];
    }
    
    //! renvoie un resume de tous les compteurs
    std::string getSummaryString( const int n= 30 ) const
    {
        char tmp[1024];
        std::string summary;
        
        for(std::map<std::string, StatsCounter *>::const_iterator 
            i= m_counters_map.begin(); i != m_counters_map.end(); ++i)
        {
            assert(i->second != NULL);
            
            const int value= i->second->last();
            float av= i->second->average(n);
            sprintf(tmp, "%s: %d [%.3f : %d]\n",
                i->first.c_str(), value, av, n);
            
            summary.append(tmp);
        }
        
        return summary;
    }
    
    //! interface du singleton.
    static
    StatsCounterIO& manager( )
    {
        static StatsCounterIO manager;
        return manager;
    }
};


//! mesure le temps d'execution d'un bloc et enregistre la mesure dans un compteur nomme.
/*! exemple d'utilisation :
\code
    {
        ScopedTimer timer("frame_time");

        // afficher l'image ...
    }
\endcode

    lorsque l'application se termine, StatsCounterIO ecrit l'historique des compteurs dans des fichiers texte et affiche un resume de leurs mesures.
*/

class ScopedTimer
{
    ProfilerClock::Ticks m_base;
    StatsCounter *m_counter;
    
    // non copyable
    ScopedTimer( const ScopedTimer& );
    ScopedTimer& operator=( const ScopedTimer& );
    
    ScopedTimer( );
    
public:
    //! constructeur, nom et taille de l'historique du compteur (gk::StatsCounter) associe au timer.
    ScopedTimer( const std::string& name, const int n= 100 )
    {
        StatsCounter *counter= StatsCounterIO::manager().find(name);
        if(counter == NULL)
            counter= StatsCounterIO::manager().create(name, n);
        
        m_counter= counter;
        m_base= ProfilerClock::getTicks();
    }
    
    //! arrete la mesure de temps, l'enregistre dans le compteur et la renvoie.
    int stop( )
    {
        int time= ProfilerClock::getDelay(m_base);
        if(m_counter != NULL)
            m_counter->push(time);
        m_counter= NULL;        // ne comptabilise pas 2 fois la mesure
        
        return time;
    }
    
    //! destructeur, termine la mesure et l'insere dans le compteur.
    ~ScopedTimer( )
    {
        int time= ProfilerClock::getDelay(m_base);
        if(m_counter != NULL)
            m_counter->push(time);
    }
};


//! creation / acces simplifie a un compteur nomme, gk::StatsCounter.
class ScopedCounter
{
    StatsCounter *m_counter;
    
    // non copyable
    ScopedCounter( const ScopedCounter& );
    ScopedCounter& operator=( const ScopedCounter& );

    ScopedCounter( );
    
public:
    //! constructeur, nomme le compteur.
    ScopedCounter( const std::string& name, const int n= 100 )
    {
        StatsCounter *counter= StatsCounterIO::manager().find(name);
        if(counter == NULL)
            counter= StatsCounterIO::manager().create(name, n);
        
        m_counter= counter;
    }

    //! ajoute une valeur a l'historique du compteur.
    void push( const int value )
    {
        if(m_counter != NULL)
            m_counter->push(value);
    }
   
    //! destructeur.
    ~ScopedCounter( ) {}
};


//! retrouve un compteur gk::StatsCounter et renvoie un resume des mesures du compteur. les mesures sont interpretees comme des durees en micro-secondes.
class TimerSummary
{
    int m_min;
    int m_max;
    float m_average;
    int m_last;
    
public:
    //! constructeur, nom du compteur a retrouver.
    TimerSummary( const std::string& name )
        :
        m_min(0),
        m_max(0),
        m_average(0.f),
        m_last(0)
    {
        StatsCounter *counter= StatsCounterIO::manager().find(name);
        if(counter == NULL)
            return;
        
        counter->getStats(&m_min, &m_average, &m_max);
        m_last= counter->last();
    }
    
    //! destructeur.
    ~TimerSummary( ) {}
    
    //! renvoie une chaine de caracteres : derniere mesure + stats du compteur.
    std::string getSummaryString( ) const
    {
        char tmp[1024];
        sprintf(tmp, "% 5dms % 4dus (min % 5dus < %.3fus < % 5dus)", m_last / 1000, m_last % 1000, 
            m_min, m_average, m_max);
        return std::string(tmp);
    }
};

//! retrouve un compteur et renvoie un resume des mesures du compteur. les mesures ne sont pas "interpretees", cf TimerSummary
class CounterSummary
{
    int m_min;
    int m_max;
    float m_average;
    int m_last;
    
public:
    //! constructeur, nom du compteur a retrouver.
    CounterSummary( const std::string& name )
        :
        m_min(0),
        m_max(0),
        m_average(0.f),
        m_last(0)
    {
        StatsCounter *counter= StatsCounterIO::manager().find(name);
        if(counter == NULL)
            return;
        
        counter->getStats(&m_min, &m_average, &m_max);
        m_last= counter->last();
    }
    
    //! destructeur.
    ~CounterSummary( ) {}
    
    //! renvoie une chaine de caracteres : derniere mesure + stats du compteur.
    std::string getSummaryString( ) const
    {
        char tmp[1024];
        sprintf(tmp, "%d (min %d < %.3f < %d)", m_last, 
            m_min, m_average, m_max);
        return std::string(tmp);
    }
};

}

#endif
